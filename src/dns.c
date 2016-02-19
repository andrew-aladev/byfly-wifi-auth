// This file is part of byfly-wifi-auth. For the copyright and license information, please view the LICENSE file that was distributed with this source code.

#include "common.h"
#include "dns.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


#define _BWA_DNS_MAX_UDP_BUFFER_SIZE 1 << 16

static
void _bwa_dns_serialize_uint8 ( uint8_t ** destination, uint8_t data )
{
    memcpy ( * destination, &data, sizeof ( data ) );
    * destination += sizeof ( data );
}

static
uint8_t _bwa_dns_deserialize_uint8 ( uint8_t ** source )
{
    uint8_t data;
    memcpy ( &data, * source, sizeof ( data ) );
    * source += sizeof ( data );
    return data;
}

static
void _bwa_dns_serialize_uint16 ( uint8_t ** destination, uint16_t data )
{
    data = htons ( data );
    memcpy ( * destination, &data, sizeof ( data ) );
    * destination += sizeof ( data );
}

static
uint16_t _bwa_dns_deserialize_uint16 ( uint8_t ** source )
{
    uint16_t data;
    memcpy ( &data, * source, sizeof ( data ) );
    * source += sizeof ( data );
    return ntohs ( data );
}

static
void _bwa_dns_serialize_uint32 ( uint8_t ** destination, uint32_t data )
{
    data = htonl ( data );
    memcpy ( * destination, &data, sizeof ( data ) );
    * destination += sizeof ( data );
}

static
uint32_t _bwa_dns_deserialize_uint32 ( uint8_t ** source )
{
    uint32_t data;
    memcpy ( &data, * source, sizeof ( data ) );
    * source += sizeof ( data );
    return ntohl ( data );
}

void bwa_dns_serialize_header ( uint8_t * destination, const bwa_dns_header * header )
{
    uint8_t * iterator = destination;

    _bwa_dns_serialize_uint16 ( &iterator, header->id );

    uint8_t uint8 = 0;
    if ( header->is_query_or_response ) {
        uint8 |= 1 << 7;
    }
    uint8 |= header->purpose_of_message << 3;
    if ( header->is_authoritive_answer ) {
        uint8 |= 1 << 2;
    }
    if ( header->is_truncated_message ) {
        uint8 |= 1 << 1;
    }
    if ( header->is_recursion_desired ) {
        uint8 |= 1;
    }
    _bwa_dns_serialize_uint8 ( &iterator, uint8 );

    uint8 = 0;
    if ( header->is_recursion_available ) {
        uint8 |= 1 << 7;
    }
    // 0 is reserved
    if ( header->is_authenticated_data ) {
        uint8 |= 1 << 5;
    }
    if ( header->is_checking_disabled ) {
        uint8 |= 1 << 4;
    }
    uint8 |= header->response_code;
    _bwa_dns_serialize_uint8 ( &iterator, uint8 );

    _bwa_dns_serialize_uint16 ( &iterator, header->questions_count );
    _bwa_dns_serialize_uint16 ( &iterator, header->answers_count );
    _bwa_dns_serialize_uint16 ( &iterator, header->authorities_count );
    _bwa_dns_serialize_uint16 ( &iterator, header->additional_records_count );
}

void bwa_dns_deserialize_header ( bwa_dns_header * header, const uint8_t * source )
{
    uint8_t * iterator = ( uint8_t * ) source;

    header->id = _bwa_dns_deserialize_uint16 ( &iterator );

    uint8_t uint8 = _bwa_dns_deserialize_uint8 ( &iterator );
    header->is_query_or_response  = ( ( uint8 >> 7 ) & 1 ) == 1;
    header->purpose_of_message    = ( ( uint8 >> 3 ) & 0xf ); // 0b1111 == 0xf
    header->is_authoritive_answer = ( ( uint8 >> 2 ) & 1 ) == 1;
    header->is_truncated_message  = ( ( uint8 >> 1 ) & 1 ) == 1;
    header->is_recursion_desired  = ( uint8 & 1 ) == 1;

    uint8 = _bwa_dns_deserialize_uint8 ( &iterator );
    header->is_recursion_available = ( ( uint8 >> 7 ) & 1 ) == 1;
    // 0 is reserved
    header->is_authenticated_data = ( ( uint8 >> 5 ) & 1 ) == 1;
    header->is_checking_disabled  = ( ( uint8 >> 4 ) & 1 ) == 1;
    header->response_code         = uint8 & 0xf; // 0b1111 == 0xf

    header->questions_count          = _bwa_dns_deserialize_uint16 ( &iterator );
    header->answers_count            = _bwa_dns_deserialize_uint16 ( &iterator );
    header->authorities_count        = _bwa_dns_deserialize_uint16 ( &iterator );
    header->additional_records_count = _bwa_dns_deserialize_uint16 ( &iterator );
}

void bwa_dns_serialize_question ( uint8_t * destination, const bwa_dns_question * question )
{
    uint8_t * iterator = destination;

    _bwa_dns_serialize_uint16 ( &iterator, question->type );
    _bwa_dns_serialize_uint16 ( &iterator, question->klass );
}

void bwa_dns_deserialize_question ( bwa_dns_question * question, const uint8_t * source )
{
    uint8_t * iterator = ( uint8_t * ) source;

    question->type  = _bwa_dns_deserialize_uint16 ( &iterator );
    question->klass = _bwa_dns_deserialize_uint16 ( &iterator );
}

void bwa_dns_serialize_response_data ( uint8_t * destination, const bwa_dns_response_data * response_data )
{
    uint8_t * iterator = destination;

    _bwa_dns_serialize_uint16 ( &iterator, response_data->type );
    _bwa_dns_serialize_uint16 ( &iterator, response_data->klass );
    _bwa_dns_serialize_uint32 ( &iterator, response_data->ttl );
    _bwa_dns_serialize_uint16 ( &iterator, response_data->data_length );
}

void bwa_dns_deserialize_response_data ( bwa_dns_response_data * response_data, const uint8_t * source )
{
    uint8_t * iterator = ( uint8_t * ) source;

    response_data->type        = _bwa_dns_deserialize_uint16 ( &iterator );
    response_data->klass       = _bwa_dns_deserialize_uint16 ( &iterator );
    response_data->ttl         = _bwa_dns_deserialize_uint32 ( &iterator );
    response_data->data_length = _bwa_dns_deserialize_uint16 ( &iterator );
}

// www.google.com -> 3www6google3com
static
char * _bwa_dns_format_host ( const char * host )
{
    size_t host_length = strlen ( host );
    char * dns_host = malloc ( host_length + 2 );
    if ( dns_host == NULL ) {
        BWA_PRINT_ERROR ( "malloc failed" );
        return NULL;
    }

    size_t index;
    size_t index_for_symbols_count = 0;
    size_t symbols_count_in_last_group = 0;
    for ( index = 0; index < host_length; index ++ ) {
        char ch = host[index];
        if ( ch == '.' ) {
            dns_host[index_for_symbols_count] = symbols_count_in_last_group;
            symbols_count_in_last_group = 0;
            index_for_symbols_count = index + 1;
        } else {
            dns_host[index + 1] = ch;
            symbols_count_in_last_group ++;
        }
    }

    // "", "www.", "www.google." are invalid hosts
    if ( symbols_count_in_last_group == 0 ) {
        BWA_FPRINT_ERROR ( "invalid host %s", host );
        free ( dns_host );
        return NULL;
    }

    dns_host[index_for_symbols_count] = symbols_count_in_last_group;
    dns_host[host_length + 1] = '\0';
    return dns_host;
}

uint8_t bwa_dns_parse_host ( const char * host_name, bwa_dns_host * host )
{
    host->name = _bwa_dns_format_host ( host_name );
    if ( host->name == NULL ) {
        return 1;
    }
    host->length = strlen ( host->name ) + 1;
    return 0;
}

void bwa_dns_free_host ( bwa_dns_host * host )
{
    free ( host->name );
}

void bwa_dns_serialize_host ( uint8_t * destination, const bwa_dns_host * host )
{
    memcpy ( destination, host->name, host->length );
}

uint8_t bwa_dns_send_query ( int socket_fd, const bwa_dns_host * host, const struct sockaddr_in * destination )
{
    bwa_dns_header header;
    header.id = getpid ();

    header.is_recursion_desired  = 1;
    header.is_truncated_message  = 0;
    header.is_authoritive_answer = 0;
    header.purpose_of_message    = 0;
    header.is_query_or_response  = 0;

    header.response_code          = 0;
    header.is_checking_disabled   = 0;
    header.is_authenticated_data  = 0;
    header.is_recursion_available = 0;

    header.questions_count          = 1;
    header.answers_count            = 0;
    header.authorities_count        = 0;
    header.additional_records_count = 0;

    bwa_dns_question question;
    question.type  = BWA_DNS_QUESTION_TYPE_A;
    question.klass = 1;

    size_t data_length = BWA_DNS_HEADER_SERIALIZED_SIZE + host->length + BWA_DNS_QUESTION_SERIALIZED_SIZE;
    uint8_t * data = malloc ( data_length );
    if ( data == NULL ) {
        BWA_PRINT_ERROR ( "malloc failed" );
        return 2;
    }

    bwa_dns_serialize_header   ( data, &header );
    bwa_dns_serialize_host     ( data + BWA_DNS_HEADER_SERIALIZED_SIZE, host );
    bwa_dns_serialize_question ( data + BWA_DNS_HEADER_SERIALIZED_SIZE + host->length, &question );

    if ( sendto ( socket_fd, data, data_length, 0, ( struct sockaddr * ) destination, sizeof ( struct sockaddr_in ) ) == -1 ) {
        BWA_PRINT_ERROR ( "sendto failed" );
        free ( data );
        return 3;
    }

    free ( data );
    return 0;
}

typedef void ( * _bwa_dns_label_symbol_callback ) ( void * user_data, char symbol );

static
uint8_t _bwa_dns_read_label_symbols ( const uint8_t * source, const uint8_t * data, size_t max_offset, size_t * source_offset, _bwa_dns_label_symbol_callback callback, void * user_data )
{
    if ( source < data ) {
        BWA_PRINT_ERROR ( "data overflow" );
        return 1;
    }
    size_t offset = source - data;
    if ( offset > max_offset ) {
        BWA_PRINT_ERROR ( "data overflow" );
        return 2;
    }

    size_t start_offset = offset;
    bool is_jumped = false;
    uint8_t byte;
    while ( ( byte = * ( data + offset ) ) != 0 ) {
        uint8_t byte_head = byte >> 6;

        if ( byte_head == 0x3 ) {
            // if byte_head is 0b11 == 0x3: byte is a part of new uint16_t offset

            size_t new_offset = ( byte & 0x3f ) << 8; // 0b111111 == 0x3f
            byte = * ( data + offset + 1 );
            new_offset |= byte;

            if ( new_offset > max_offset ) {
                BWA_PRINT_ERROR ( "data overflow" );
                return 3;
            }
            offset = new_offset;
            is_jumped = true;
        } else {
            // byte is a part of label

            callback ( user_data, ( char ) byte );
            if ( offset + 1 > max_offset ) {
                BWA_PRINT_ERROR ( "data overflow" );
                return 4;
            }
            offset ++;
        }
    }
    callback ( user_data, '\0' );

    if ( is_jumped ) {
        * source_offset = 2;
    } else {
        * source_offset = offset - start_offset;
    }

    return 0;
}

static
void _bwa_dns_label_count_symbols ( void * user_data, char BWA_UNUSED ( symbol ) )
{
    size_t * label_length = user_data;
    ( * label_length ) ++;
}

static
void _bwa_dns_label_store_symbols ( void * user_data, char symbol )
{
    bwa_dns_label * label = user_data;
    label->text[label->length] = symbol;
    label->length ++;
}

uint8_t bwa_dns_read_label ( bwa_dns_label * label, const uint8_t * source, const uint8_t * data, size_t max_offset, size_t * source_offset )
{
    size_t label_length = 0;
    if ( _bwa_dns_read_label_symbols ( source, data, max_offset, source_offset, _bwa_dns_label_count_symbols, &label_length ) != 0 ) {
        return 1;
    }
    label->length = 0;
    label->text   = malloc ( label_length );
    if ( label->text == NULL ) {
        BWA_PRINT_ERROR ( "malloc failed" );
        return 2;
    }
    if ( _bwa_dns_read_label_symbols ( source, data, max_offset, source_offset, _bwa_dns_label_store_symbols, label ) != 0 ) {
        free ( label->text );
        return 3;
    }

    return 0;
}

uint8_t bwa_dns_ignore_label ( const uint8_t * source, const uint8_t * data, size_t max_offset, size_t * source_offset )
{
    size_t label_length;
    if ( _bwa_dns_read_label_symbols ( source, data, max_offset, source_offset, _bwa_dns_label_count_symbols, &label_length ) != 0 ) {
        return 1;
    }
    return 0;
}

void bwa_dns_free_label ( bwa_dns_label * label )
{
    free ( label->text );
}

uint8_t bwa_dns_receive_data ( int socket_fd, const bwa_dns_host * host, const struct sockaddr_in * destination, struct in_addr * result_address )
{
    uint8_t * data = malloc ( _BWA_DNS_MAX_UDP_BUFFER_SIZE );
    if ( data == NULL ) {
        BWA_PRINT_ERROR ( "malloc failed" );
        return 1;
    }

    socklen_t destination_length = sizeof ( struct sockaddr_in );
    size_t data_length = BWA_DNS_HEADER_SERIALIZED_SIZE + host->length + BWA_DNS_QUESTION_SERIALIZED_SIZE;
    ssize_t received_length = recvfrom ( socket_fd, data, _BWA_DNS_MAX_UDP_BUFFER_SIZE, 0, ( struct sockaddr * ) destination, &destination_length );
    if ( ( size_t ) received_length < data_length ) {
        BWA_PRINT_ERROR ( "recvfrom failed" );
        free ( data );
        return 2;
    }

    bwa_dns_header header;
    bwa_dns_deserialize_header ( &header, data );
    if ( header.answers_count == 0 ) {
        BWA_PRINT_ERROR ( "no answers found" );
        free ( data );
        return 3;
    }

    uint8_t * iterator = data + data_length;

    size_t index;
    size_t source_offset;
    for ( index = 0; index < header.answers_count; index ++ ) {
        bwa_dns_label label;
        if ( bwa_dns_read_label ( &label, iterator, data, received_length - 1, &source_offset ) != 0 ) {
            free ( data );
            return 4;
        }
        iterator += source_offset;

        bwa_dns_response_data response_data;
        bwa_dns_deserialize_response_data ( &response_data, iterator );
        iterator += BWA_DNS_RESPONSE_DATA_SERIALIZED_SIZE;

        if ( response_data.type == BWA_DNS_QUESTION_TYPE_A ) {
            if ( strcmp ( label.text, host->name ) == 0 ) {
                memcpy ( &result_address->s_addr, iterator, response_data.data_length );
                bwa_dns_free_label ( &label );
                break;
            }
            iterator += response_data.data_length;
        } else {
            // ignore alias name
            if ( bwa_dns_ignore_label ( iterator, data, received_length - 1, &source_offset ) != 0 ) {
                bwa_dns_free_label ( &label );
                free ( data );
                return 5;
            }
            iterator += source_offset;
        }

        bwa_dns_free_label ( &label );
    }

    free ( data );
    return 0;
}

uint8_t bwa_dns_query ( const char * host_name, const char * dns_ip, char ** result_address )
{
    int socket_fd = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if (socket_fd == -1) {
        BWA_PRINT_ERROR ( "socket creation failed" );
        return 1;
    }

    struct timeval tv;
    tv.tv_sec  = 10;
    tv.tv_usec = 0;
    if (
        setsockopt ( socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof ( tv ) ) == -1 ||
        setsockopt ( socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof ( tv ) ) == -1
    ) {
        BWA_PRINT_ERROR ( "setsockopt failed" );
        close ( socket_fd );
       	return 2;
    }

    struct sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_port = htons ( 53 );
    if ( inet_aton ( dns_ip, &destination.sin_addr ) == 0 ) {
        BWA_PRINT_ERROR ( "inet_aton failed" );
        close ( socket_fd );
        return 3;
    }

    bwa_dns_host host;
    if ( bwa_dns_parse_host ( host_name, &host ) != 0 ) {
        bwa_dns_free_host ( &host );
        close ( socket_fd );
        return 4;
    }

    if ( bwa_dns_send_query ( socket_fd, &host, &destination ) != 0 ) {
        bwa_dns_free_host ( &host );
        close ( socket_fd );
        return 5;
    }

    struct in_addr address_data;
    if ( bwa_dns_receive_data ( socket_fd, &host, &destination, &address_data ) != 0 ) {
        bwa_dns_free_host ( &host );
        close ( socket_fd );
        return 6;
    }

    char * address = inet_ntoa ( address_data );
    if ( address == NULL ) {
        BWA_PRINT_ERROR ( "inet_ntoa failed" );
        bwa_dns_free_host ( &host );
        close ( socket_fd );
        return 7;
    }
    * result_address = strdup ( address );

    bwa_dns_free_host ( &host );
    close ( socket_fd );
    return 0;
}
