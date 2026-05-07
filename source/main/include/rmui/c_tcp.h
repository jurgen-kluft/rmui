#ifndef __ARDUINO_REMOTE_UI_TCP_H__
#define __ARDUINO_REMOTE_UI_TCP_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace ntcp
    {
        // ------------------------------------------------------------
        // Basic types assumed provided elsewhere:
        // u8, u16, u32, i32, etc.
        // g_memset, g_memcpy
        // ------------------------------------------------------------

        // ------------------------------------------------------------
        // State
        // ------------------------------------------------------------

        enum tcp_state_t
        {
            TCP_STATE_DISCONNECTED = 0,
            TCP_STATE_CONNECTING,
            TCP_STATE_CONNECTED,
            TCP_STATE_BACKOFF
        };

        // ------------------------------------------------------------
        // Function pointer typedefs
        // ------------------------------------------------------------

        typedef u32 (*millis_fn)();

        typedef bool (*tcp_connect_fn)(void* sock, const char* host, u16 port);
        typedef bool (*tcp_connected_fn)(void* sock);
        typedef int (*tcp_available_fn)(void* sock);
        typedef int (*tcp_read_fn)(void* sock, void* dst, u32 len);
        typedef int (*tcp_write_fn)(void* sock, const void* src, u32 len);
        typedef void (*tcp_stop_fn)(void* sock);

        typedef u8* (*rx_acquire_fn)(void* ctx, u16 len);
        typedef void (*rx_commit_fn)(void* ctx, u8* buf, u16 len);
        typedef void (*rx_abort_fn)(void* ctx);

        // ------------------------------------------------------------
        // Ops groupings
        // ------------------------------------------------------------

        struct time_ops_t
        {
            millis_fn m_millis;
        };

        void init_time_ops_for_arduino(time_ops_t* ops);

        struct tcp_socket_ops_t
        {
            tcp_connect_fn   m_connect;
            tcp_connected_fn m_connected;
            tcp_available_fn m_available;
            tcp_read_fn      m_read;
            tcp_write_fn     m_write;
            tcp_stop_fn      m_stop;
        };

        void init_tcp_socket_ops_for_arduino(tcp_socket_ops_t* ops);

        struct rx_ops_t
        {
            rx_acquire_fn m_acquire;
            rx_commit_fn  m_commit;
            rx_abort_fn   m_abort;
        };

        struct tcp_timing_t
        {
            u32 m_connect_timeout_ms;
            u32 m_backoff_initial_ms;
            u32 m_backoff_max_ms;
        };

        inline void init_tcp_timing(tcp_timing_t* timing)
        {
            // default timing values
            timing->m_connect_timeout_ms = 5000;       // 5 seconds
            timing->m_backoff_initial_ms = 1000;       // 1 second
            timing->m_backoff_max_ms     = 5 * 60000;  // 5 minutes
        }

        // ------------------------------------------------------------
        // TCP client
        // ------------------------------------------------------------
        struct config_t
        {
            time_ops_t       m_time_ops;
            tcp_socket_ops_t m_sock_ops;
            rx_ops_t         m_rx_ops;
            tcp_timing_t     m_timing;
        };

        struct tcp_client_t
        {
            void*       m_socket;
            const char* m_host;
            u16         m_port;

            // config
            const config_t* m_config;

            // runtime
            tcp_state_t m_state;
            u32         m_last_attempt_ms;
            u32         m_connect_start_ms;
            u32         m_backoff_ms;

            // rx framing state
            void* m_rx_ctx;
            u16   m_rx_expected;
            u16   m_rx_offset;
            u8*   m_rx_buf;
        };

        // ------------------------------------------------------------
        // API
        // ------------------------------------------------------------
        void*       m_socket;
        const char* m_host;
        u16         m_port;

        void init(tcp_client_t* c, const config_t* config, void* socket, const char* host, u16 port, void* rx_ctx);
        void tick(tcp_client_t* c);
        bool is_connected(const tcp_client_t* c);
        void send(tcp_client_t* c, const void* data, u16 len);

    }  // namespace ntcp
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_TCP_H__