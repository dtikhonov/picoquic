/*
* Author: Christian Huitema
* Copyright (c) 2019, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
* Packet logging.
*/
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "picoquic_internal.h"

typedef enum {
    picoquic_log_event_pdu_sent = 0x0002,
    picoquic_log_event_pdu_recv = 0x0003,

    picoquic_log_event_packet_sent = 0x0008,
    picoquic_log_event_packet_recv = 0x0009,

    picoquic_log_event_new_connection = 0x0010,
    picoquic_log_event_connection_close = 0x0011,
    picoquic_log_event_connection_id_update = 0x0012,

    picoquic_log_event_tls_key_update = 0x0020,
    picoquic_log_event_tls_key_retired = 0x0021,

    picoquic_log_event_version_update = 0x0035,
    picoquic_log_event_param_update = 0x0036,
    picoquic_log_event_alpn_update = 0x0037,
    picoquic_log_event_cc_update = 0x0038,
    picoquic_log_event_stream_update = 0x0039,

    picoquic_log_event_frame_sent = 0x0082,
    picoquic_log_event_frame_recv = 0x0083,
} picoquic_log_event_type;

/* binary alternative to picoquic_log_packet_address() */
void binlog_pdu(FILE * f, const picoquic_connection_id_t* cid, int receiving, uint64_t current_time,
    const struct sockaddr* addr_peer, size_t packet_length);

/* binary alternative to picoquic_log_decrypted_segment() */
void binlog_packet(FILE * f, const picoquic_connection_id_t* cid, int receiving, uint64_t current_time,
    const picoquic_packet_header * ph, const uint8_t* bytes, size_t bytes_max);

/* binary alternative to picoquic_log_outgoing_segment() */
void binlog_outgoing_packet(FILE * f, picoquic_cnx_t* cnx,
    uint8_t * bytes, uint64_t sequence_number, size_t pn_length, size_t length,
    uint8_t* send_buffer, size_t send_length, uint64_t current_time);

/* binary alternative to picoquic_log_transport_extension() */
void binlog_transport_extension(FILE * f, picoquic_cnx_t * cnx);

/* binary alternative to picoquic_log_tls_ticket() */
void binlog_picotls_ticket(FILE* f, picoquic_connection_id_t cnx_id,
    uint8_t* ticket, uint16_t ticket_length);

void binlog_new_connection(picoquic_cnx_t * cnx);
void binlog_close_connection(picoquic_cnx_t * cnx);

int binlog_open(picoquic_quic_t * quic, char const * binlog_file);
void binlog_close(picoquic_quic_t * quic);

void picoquic_cc_dump(picoquic_cnx_t * cnx, uint64_t current_time);
