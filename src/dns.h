// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#if !defined ( _BWA_DNS_H )
#define _BWA_DNS_H

#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>


// Documentation: RFC 1035 and 4035

/*
field                    | amount of bits
-----------------------------------------
id                       | 16

is_query_or_response     | 1
purpose_of_message       | 4
is_authoritive_answer    | 1
is_truncated_message     | 1
is_recursion_desired     | 1

is_recursion_available   | 1
--reserved--             | 1
is_authenticated_data    | 1
is_checking_disabled     | 1
response_code            | 4

questions_count          | 16
answers_count            | 16
authorities_count        | 16
additional_records_count | 16
-----------------------------------------
total                    | 12 * 8 bits
*/

#define BWA_DNS_HEADER_SERIALIZED_SIZE 12

typedef struct bwa_dns_header_t
{
    uint16_t id;

    bool    is_query_or_response;
    uint8_t purpose_of_message;
    bool    is_authoritive_answer;
    bool    is_truncated_message;
    bool    is_recursion_desired;

    bool    is_recursion_available;
    bool    is_authenticated_data;
    bool    is_checking_disabled;
    uint8_t response_code;

    uint16_t questions_count;
    uint16_t answers_count;
    uint16_t authorities_count;
    uint16_t additional_records_count;
} bwa_dns_header;

void bwa_dns_serialize_header   ( uint8_t * destination, const bwa_dns_header * header );
void bwa_dns_deserialize_header ( bwa_dns_header * header, const uint8_t * source );

#define BWA_DNS_QUESTION_SERIALIZED_SIZE 4

typedef struct bwa_dns_question_t
{
    uint16_t type;
    uint16_t klass;
} bwa_dns_question;

#define BWA_DNS_QUESTION_TYPE_A 1

void bwa_dns_serialize_question   ( uint8_t * destination, const bwa_dns_question * question );
void bwa_dns_deserialize_question ( bwa_dns_question * question, const uint8_t * source );

typedef struct bwa_dns_response_data_t
{
    uint16_t type;
    uint16_t klass;
    uint32_t ttl;
    uint16_t data_length;
} bwa_dns_response_data;

#define BWA_DNS_RESPONSE_DATA_SERIALIZED_SIZE 10

void bwa_dns_serialize_response_data   ( uint8_t * destination, const bwa_dns_response_data * response_data );
void bwa_dns_deserialize_response_data ( bwa_dns_response_data * response_data, const uint8_t * source );

typedef struct bwa_dns_host_t {
    char * name;
    size_t length;
} bwa_dns_host;

uint8_t bwa_dns_parse_host     ( const char * host_name, bwa_dns_host * host );
void    bwa_dns_free_host      ( bwa_dns_host * host );
void    bwa_dns_serialize_host ( uint8_t * destination, const bwa_dns_host * host );

typedef struct bwa_dns_label_t {
    char * text;
    size_t length;
} bwa_dns_label;

uint8_t bwa_dns_read_label   ( bwa_dns_label * label, const uint8_t * source, const uint8_t * data, size_t max_offset, size_t * source_offset );
uint8_t bwa_dns_ignore_label ( const uint8_t * source, const uint8_t * data, size_t max_offset, size_t * source_offset );
void    bwa_dns_free_label   ( bwa_dns_label * label );

uint8_t bwa_dns_send_query   ( int socket_fd, const bwa_dns_host * host, const struct sockaddr_in * destination );
uint8_t bwa_dns_receive_data ( int socket_fd, const bwa_dns_host * host, const struct sockaddr_in * destination, struct in_addr * result_address );
uint8_t bwa_dns_query        ( const char * host_name, const char * dns_ip, char ** result_address );


#endif
