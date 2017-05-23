#ifndef GENERALS_HEADERFILE_H
#define GENERALS_HEADERFILE_H
#include"messages.h"
#include <sys/socket.h>
#include <netinet/in.h>
#define MAX_MESSAGE_SIZE 1024

class generals{

  public:
    
    GENERAL_ID general_id; //Could be Caeser or any of the generals
    GENERAL_ID observar_general;
    int snapshot_marker;
    int update_vector_clock(int *received_clock);
    int process_msg_sent_before_snapshot(GENERAL_ID sender,UNIT_TYPES army_unit,int num_of_troops);
    CHANNEL_ID get_channel_id(GENERAL_ID,GENERAL_ID);
    int send_own_state(GENERAL_ID);
    int global_snapshot();
    int process_snapshot_initiated_msg(GENERAL_ID);
    int send_marker_to_all_other_processes(GENERAL_ID);
    int process_snapshot_message_general(GENERAL_ID, army_units);
    int initiate_snapshot();
    int forward_msg_received_to_observar();
    int update_global_state();
    int process_snapshot_message_channel(CHANNEL_ID,UNIT_TYPES type,int number_of_units);
    int increment_vector_clock();
    int send_current_army_unit_info(GENERAL_ID);
    int send_army_units_to_general(GENERAL_ID);
    int recive_army_units_from_general(GENERAL_ID);
    int * current_vector_clock();
    int init_network_connection();
    char * get_hostname_from_general_id(GENERAL_ID ID);
    int create_check_troop_status_message(char *msg_buffer);
    int send_message_udp(GENERAL_ID,char *);
     int initialize_process();
    int update_troop_count(UNIT_TYPES unit_type, int num_units,UPDATE_TROOP type );
    int send_units_to_general(GENERAL_ID send_to, UNIT_TYPES troop_type,int num_of_troops);
    int receive_units_from_general(GENERAL_ID , UNIT_TYPES ,int );
    int print_number_of_troops(army_units *units);
    int send_unit_count_to_caesar();
    int set_total_number_of_troops();
    int start_general_process();
    int eqaulize_unit(int units[],UNIT_TYPES unit_type);
    int redistribute_troops();
    int start_caeser_process();
    int create_send_troop_message(char *msg_buffer,GENERAL_ID send_msg_to, GENERAL_ID send_troops_to,
                                          UNIT_TYPES type,int num_of_units);
  private:
    army_units my_units;
    army_units total_global_units;
    army_units snapshot_global_units;
    army_unit all_generals[4];
    army_unit all_channels[6];
    army_unit snapshot_generals[4];
    int my_vector_clock[4];
    int p_port;
    int p_socket;
    GLOBAL_STATE g_state;
    int my_state[5];
     struct  sockaddr_in addr;


};

#endif 
