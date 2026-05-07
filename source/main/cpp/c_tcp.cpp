#include "rmui/c_rmui.h"
#include "rmui/c_tcp.h"

namespace ncore
{
    namespace ntcp
    {
        // ------------------------------------------------------------
        // Static helpers
        // ------------------------------------------------------------

        static bool s_read_u16(tcp_client_t* c, u16* out)
        {
            if (c->m_config->m_sock_ops.m_available(c->m_socket) < 2)
                return false;

            u8 hdr[2];
            c->m_config->m_sock_ops.m_read(c->m_socket, hdr, 2);
            *out = (u16(hdr[0]) << 8) | hdr[1];
            return true;
        }

        static void s_enter_backoff(tcp_client_t* c)
        {
            c->m_state = TCP_STATE_BACKOFF;

            u32 next = c->m_backoff_ms << 1;
            if (next > c->m_config->m_timing.m_backoff_max_ms)
                next = c->m_config->m_timing.m_backoff_max_ms;

            c->m_backoff_ms      = next;
            c->m_last_attempt_ms = c->m_config->m_time_ops.m_millis();
        }

        static void s_attempt_connect(tcp_client_t* c)
        {
            u32 now = c->m_config->m_time_ops.m_millis();
            if ((now - c->m_last_attempt_ms) < c->m_backoff_ms)
                return;

            c->m_last_attempt_ms  = now;
            c->m_connect_start_ms = now;
            c->m_state            = TCP_STATE_CONNECTING;

            c->m_config->m_sock_ops.m_stop(c->m_socket);
            c->m_config->m_sock_ops.m_connect(c->m_socket, c->m_host, c->m_port);
        }

        static void s_poll_connecting(tcp_client_t* c)
        {
            if (c->m_config->m_sock_ops.m_connected(c->m_socket))
            {
                c->m_state       = TCP_STATE_CONNECTED;
                c->m_backoff_ms  = c->m_config->m_timing.m_backoff_initial_ms;
                c->m_rx_expected = 0;
                c->m_rx_offset   = 0;
                c->m_rx_buf      = 0;
                return;
            }

            if ((c->m_config->m_time_ops.m_millis() - c->m_connect_start_ms) > c->m_config->m_timing.m_connect_timeout_ms)
            {
                c->m_config->m_sock_ops.m_stop(c->m_socket);
                s_enter_backoff(c);
            }
        }

        static void s_poll_backoff(tcp_client_t* c)
        {
            if ((c->m_config->m_time_ops.m_millis() - c->m_last_attempt_ms) >= c->m_backoff_ms)
            {
                c->m_state = TCP_STATE_DISCONNECTED;
            }
        }

        static void s_poll_connected(tcp_client_t* c)
        {
            if (!c->m_config->m_sock_ops.m_connected(c->m_socket))
            {
                if (c->m_rx_buf && c->m_config->m_rx_ops.m_abort)
                    c->m_config->m_rx_ops.m_abort(c->m_rx_ctx);

                c->m_rx_buf = 0;
                c->m_config->m_sock_ops.m_stop(c->m_socket);
                s_enter_backoff(c);
                return;
            }

            while (c->m_config->m_sock_ops.m_available(c->m_socket) > 0)
            {
                if (c->m_rx_expected == 0)
                {
                    if (!s_read_u16(c, &c->m_rx_expected))
                        return;

                    c->m_rx_offset = 0;
                    c->m_rx_buf    = c->m_config->m_rx_ops.m_acquire(c->m_rx_ctx, c->m_rx_expected);

                    if (!c->m_rx_buf)
                    {
                        if (c->m_config->m_rx_ops.m_abort)
                            c->m_config->m_rx_ops.m_abort(c->m_rx_ctx);

                        c->m_rx_expected = 0;
                        return;
                    }
                }

                u16 remaining = c->m_rx_expected - c->m_rx_offset;
                u16 avail     = (u16)c->m_config->m_sock_ops.m_available(c->m_socket);
                u16 chunk     = remaining < avail ? remaining : avail;

                c->m_config->m_sock_ops.m_read(c->m_socket, c->m_rx_buf + c->m_rx_offset, chunk);

                c->m_rx_offset += chunk;

                if (c->m_rx_offset == c->m_rx_expected)
                {
                    c->m_config->m_rx_ops.m_commit(c->m_rx_ctx, c->m_rx_buf, c->m_rx_expected);

                    c->m_rx_expected = 0;
                    c->m_rx_offset   = 0;
                    c->m_rx_buf      = 0;
                }
            }
        }

        // ------------------------------------------------------------
        // Public API
        // ------------------------------------------------------------

        void init(tcp_client_t* c, const config_t* config, void* socket, const char* host, u16 port, void* rx_ctx)
        {
            c->m_socket = socket;
            c->m_host   = host;
            c->m_port   = port;

            c->m_config = config;

            c->m_state            = TCP_STATE_DISCONNECTED;
            c->m_last_attempt_ms  = 0;
            c->m_connect_start_ms = 0;
            c->m_backoff_ms       = config->m_timing.m_backoff_initial_ms;

            c->m_rx_ctx      = rx_ctx;
            c->m_rx_expected = 0;
            c->m_rx_offset   = 0;
            c->m_rx_buf      = 0;
        }

        void tick(tcp_client_t* c)
        {
            switch (c->m_state)
            {
                case TCP_STATE_DISCONNECTED: s_attempt_connect(c); break;
                case TCP_STATE_CONNECTING: s_poll_connecting(c); break;
                case TCP_STATE_CONNECTED: s_poll_connected(c); break;
                case TCP_STATE_BACKOFF: s_poll_backoff(c); break;
            }
        }

        bool is_connected(const tcp_client_t* c) { return c->m_state == TCP_STATE_CONNECTED; }

        void send(tcp_client_t* c, const void* data, u16 len)
        {
            if (c->m_state != TCP_STATE_CONNECTED)
                return;

            u8 hdr[2];
            hdr[0] = (u8)(len >> 8);
            hdr[1] = (u8)(len & 0xff);

            c->m_config->m_sock_ops.m_write(c->m_socket, hdr, 2);
            c->m_config->m_sock_ops.m_write(c->m_socket, data, len);
        }

    }  // namespace ntcp
}  // namespace ncore
