#include "rmui/c_rmui.h"
#include "rmui/c_tcp.h"

#include <Arduino.h>
#include <WiFi.h>  // ESP32

namespace ncore
{
    namespace ntcp
    {
        
        static u32 millis_cb() { return (u32)::millis(); }

        void init_time_ops_for_arduino(time_ops_t* ops) { ops->m_millis = millis_cb; }

        static bool wifi_connect(void* socket, const char* host, u16 port)
        {
            WiFiClient* c = (WiFiClient*)socket;
            return c->connect(host, port);
        }

        static bool wifi_connected(void* socket)
        {
            WiFiClient* c = (WiFiClient*)socket;
            return c->connected();
        }

        static int wifi_available(void* socket)
        {
            WiFiClient* c = (WiFiClient*)socket;
            return c->available();
        }

        static int wifi_read(void* socket, void* dst, u32 len)
        {
            WiFiClient* c = (WiFiClient*)socket;
            return c->read((u8*)dst, len);
        }

        static int wifi_write(void* socket, const void* src, u32 len)
        {
            WiFiClient* c = (WiFiClient*)socket;
            return c->write((const u8*)src, len);
        }

        static void wifi_stop(void* sock)
        {
            WiFiClient* c = (WiFiClient*)sock;
            c->stop();
        }

        void init_tcp_socket_ops_for_arduino(tcp_socket_ops_t* ops)
        {
            ops->m_connect   = wifi_connect;
            ops->m_connected = wifi_connected;
            ops->m_available = wifi_available;
            ops->m_read      = wifi_read;
            ops->m_write     = wifi_write;
            ops->m_stop      = wifi_stop;
        }

        // EXAMPLE
        // rx buffer management (zero-copy)

        static u8   g_rx_storage[2048];
        static bool g_rx_in_use = false;

        static u8* rx_acquire(void* ctx, u16 len)
        {
            if (g_rx_in_use)
                return 0;

            if (len > sizeof(g_rx_storage))
                return 0;

            g_rx_in_use = true;
            return g_rx_storage;
        }

        static void rx_commit(void* ctx, u8* buffer, u16 len)
        {
            // process message here
            g_rx_in_use = false;
        }

        static void rx_abort(void* ctx) { g_rx_in_use = false; }

    }  // namespace ntcp
}  // namespace ncore
