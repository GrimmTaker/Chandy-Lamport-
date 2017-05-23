
#ifndef MESSAGES_HEADERFILE_H
#define MESSAGES_HEADERFILE_H
#include<stdio.h>

typedef struct global_state{
  int troops_on_generals[4][5];
  int troops_in_channel[6][5]; //6 channels in the system, C-B, C-O,C-P,B-P,B-O,P-O
}GLOBAL_STATE;

typedef enum snapshot_marker_state
{
    MARKER_UNSET,
    MARKER_SET,
    MARKER_RESET
}snap_shot_state;
typedef enum channel_id{
  CAESAR_POMPUS,
  CAESAR_BRUTUS,
  CAESAR_OPERACHORUS,
  POMPUS_BRUTUS,
  POMPUS_OPERACHORUS,
  BRUTUS_OPERACHORUS
}CHANNEL_ID;


typedef enum units_type{
CATAPULTS,
ARCHERS,
CAVALRY,
SPEARMEN,
INFANTRY
}UNIT_TYPES;

typedef enum message_type{
SEND_UNITS,
RECEIVE_UNITS,
RECEIVE_UNIT_COUNT,
SEND_UNIT_COUNT,
SNAPSHOT_MARKER,
SNAPSHOT_INITIATED,
SNAPSHOT_STATE_GENERAL,
SNAPSHOT_STATE_CHANNEL
}MESSAGE_TYPE;

typedef enum general_id{
CAESAR,
POMPUS,
BRUTUS,
OPERACHORUS
}GENERAL_ID;

typedef struct army_unit{
int catapults;
int archers;
int cavalry;
int spearmen;
int infantry;
}army_units;

typedef enum update_troops{
  ADD_TROOPS,
  REMOVE_TROOPS}UPDATE_TROOP;
typedef struct message{

MESSAGE_TYPE message_type;
GENERAL_ID sender_id;
GENERAL_ID dest_id;
GENERAL_ID observer_id;
CHANNEL_ID channel_num;
int marker;
int vector_clock[4];
army_units my_units;
GENERAL_ID send_to;
UNIT_TYPES troop_type;
int num_of_troops;
}MESSAGE;

#endif 
