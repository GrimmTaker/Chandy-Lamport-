#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"messages.h"
#include"class_generals.h"


int main()
{
    char node_name[104];	
    int error;
    generals general;
    general.initialize_process();

    if(general.general_id == CAESAR)
    {
        general.start_caeser_process();
    }
    else
        general.start_general_process();
}


int generals::start_general_process()
{
    int ret;
    MESSAGE *recv_msg=NULL;
    recv_msg = (MESSAGE*)malloc(sizeof(MESSAGE));

    printf("\n Starting process general");
    if(recv_msg == NULL)
    {
        printf("\n Memory allocation failed, bailing out....");
        return 1;
    }

    ret =init_network_connection();
    increment_vector_clock();
    while(1)
    {	
        struct	sockaddr_in client; // client's IP address
        socklen_t	client_len; // length of client's addr
        char	buf[ MAX_MESSAGE_SIZE ];	// data buffer
        int	len;		// buffer length
        int ret;
        //
        // Get a datagram, and print its contents
        //
        printf( "\nWaiting for a orders ...\n" );
        memset( &client, 0, sizeof( client ) );
        client_len = sizeof( client );
        len = recvfrom( p_socket, buf, sizeof( buf ), 0,
                (struct sockaddr *)&client, &client_len );
        if( len < 0 ){
            perror( "recvfrom" );
            return 1;
        }
        /*printf( "\n Client %s.%d sent: %s", inet_ntoa( client.sin_addr ),
          client.sin_port, buf );*/

        memcpy( recv_msg, buf, sizeof( MESSAGE ) );
        update_vector_clock(recv_msg->vector_clock);

        if((recv_msg->message_type == SNAPSHOT_INITIATED))
        {
            printf("\n Received snapshot initiated from %s",get_hostname_from_general_id(recv_msg->sender_id));
            increment_vector_clock();
            process_snapshot_initiated_msg(recv_msg->observer_id);

        }
        if((snapshot_marker == MARKER_SET) && (recv_msg->marker==MARKER_UNSET) &&
                (recv_msg->message_type == RECEIVE_UNITS))
        {
            printf("\n RECEIVE UNITS message received after the snapshot initiation:forwarding it");
            process_msg_sent_before_snapshot(recv_msg->sender_id,recv_msg->troop_type, recv_msg->num_of_troops);
        }


        if(recv_msg->message_type == SEND_UNIT_COUNT)
        {
            //printf("Recived request to send troop count from %s",get_hostname_from_general_id(recv_msg->sender_id));
            send_unit_count_to_caesar();
        }

        if(recv_msg->message_type == SEND_UNITS)
        {
            //printf("Recived request to send troops to %s from %s",get_hostname_from_general_id(recv_msg->send_to),get_hostname_from_general_id(recv_msg->sender_id));
            send_units_to_general(recv_msg->send_to,recv_msg->troop_type,recv_msg->num_of_troops);
        }
        if((recv_msg->message_type == RECEIVE_UNITS))
        {
            //printf("\n Received Unit from %s",get_hostname_from_general_id(recv_msg->sender_id));
            increment_vector_clock();
            receive_units_from_general(recv_msg->send_to,recv_msg->troop_type,recv_msg->num_of_troops);
        }

    }
}

