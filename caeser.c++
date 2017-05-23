#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include"messages.h"
#include"class_generals.h"






int generals::start_caeser_process()
{
    int ret;
    printf("\nStarting Process caeser\n");
    ret =init_network_connection();
    increment_vector_clock();
    //Send a troop status message

    char * msg;
    msg= (char *)malloc(sizeof(MESSAGE));
    int counter =0;
    counter ++;
    for(int i=1;i<4;i++)
    {
        memset(msg,0,sizeof(MESSAGE));
        ret = create_check_troop_status_message(msg);  
        ret=send_message_udp((GENERAL_ID)(i),msg);
    }
    //Now collect all 4 
    printf("\n Waiting to hear back from all processes");  
    int	len;		// buffer length
    MESSAGE *recv_msg=NULL;
    recv_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    struct	sockaddr_in client; // client's IP address
    socklen_t	client_len; // length of client's addr
    char	buf[ MAX_MESSAGE_SIZE ];	// data buffer
    for(int i=0;i<3;i++)
    {
        printf("\n Waiting to hear from the generals");
        memset( &client, 0, sizeof( client ) );
        client_len = sizeof( client );
        len = recvfrom( p_socket, buf, sizeof( buf ), 0,
                (struct sockaddr *)&client, &client_len );
        //memset(recv_msg,0,sizeof( MESSAGE ));

        memcpy( recv_msg, &buf, sizeof( MESSAGE ) );
        update_vector_clock(recv_msg->vector_clock);
        //printf("Recived a message from %d,%s",recv_msg->sender_id,get_hostname_from_general_id(recv_msg->sender_id));

        increment_vector_clock();

        memcpy(&all_generals[recv_msg->sender_id],&recv_msg->my_units,sizeof(army_units));

    }
    /*
       if(recv_msg->message_type == SEND_UNITS)
       {
       printf("Recived request to send troops to %s from %s",get_hostname_from_general_id(recv_msg->send_to),get_hostname_from_general_id(recv_msg->sender_id));
       increment_vector_clock();

       send_units_to_general(recv_msg->send_to,recv_msg->troop_type,recv_msg->num_of_troops);
       }
     */
    //Practice snapshot code




    printf("\nReceieved Status messages from all the generals, updating global system state");

    increment_vector_clock();
    set_total_number_of_troops();
    printf("\nNow will attempt to equalize the number of troops across all generals");
    redistribute_troops();

    ret= initiate_snapshot();
    while(1)
    {
        //printf("\n Now will update my own troops");
        memset( &client, 0, sizeof( client ) );
        client_len = sizeof( client );
        len = recvfrom( p_socket, buf, sizeof( buf ), 0,
                (struct sockaddr *)&client, &client_len );
        update_vector_clock(recv_msg->vector_clock);
        memcpy( recv_msg, buf, sizeof( MESSAGE ) );
        if(recv_msg->message_type == RECEIVE_UNIT_COUNT)
        {

            printf("\n RECIVED unit count messasge is unexpected");
            update_vector_clock(recv_msg->vector_clock);


        }

        if(recv_msg->message_type == SEND_UNITS)
        {
            //printf("Recived request to send troops to %s from %s",get_hostname_from_general_id(recv_msg->send_to),get_hostname_from_general_id(recv_msg->sender_id));
            send_units_to_general(recv_msg->send_to,recv_msg->troop_type,recv_msg->num_of_troops);
        }
        if((recv_msg->message_type == RECEIVE_UNITS))
        {
            if((snapshot_marker == MARKER_SET) && (recv_msg->marker==MARKER_UNSET) &&
                    (recv_msg->message_type == RECEIVE_UNITS))
            {
                printf("\n RECEIVE UNITS message received after the snapshot");
                
                
                CHANNEL_ID chan_id = get_channel_id(recv_msg->sender_id, general_id);
                process_snapshot_message_channel(chan_id,recv_msg->troop_type,recv_msg->num_of_troops);
            }
            //printf("\n Received Units from %s",get_hostname_from_general_id(recv_msg->sender_id));
            increment_vector_clock();
            receive_units_from_general(recv_msg->send_to,recv_msg->troop_type,recv_msg->num_of_troops);
        }
        if(recv_msg->message_type ==SNAPSHOT_STATE_GENERAL)
        {   
            printf("\n Receieved snapshot of the general %d",recv_msg->sender_id);
            process_snapshot_message_general(recv_msg->sender_id,recv_msg->my_units);
        }
        if(recv_msg->message_type ==SNAPSHOT_STATE_CHANNEL)
        {
            printf("\n Receieved snapshot of the channel %d",recv_msg->channel_num);
            process_snapshot_message_channel(recv_msg->channel_num,recv_msg->troop_type,recv_msg->num_of_troops);

        }

    }


}


 
