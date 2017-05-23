
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "class_generals.h"
#include "messages.h"
#define	PORT_MIN	1024
#define	PORT_MAX	65535

int generals::init_network_connection()
{   
    int port;
    p_port = 3001;
    port = p_port;
    if( port < PORT_MIN || port > PORT_MAX ){
        exit( EXIT_FAILURE );
    }

    //
    // Create a datagram socket.  The protocol argument is left 0 to
    // allow the system to choose an appropriate protocol.
    //
    p_socket = socket( AF_INET, SOCK_DGRAM, 0 );
    if( p_socket < 0 ){
        perror( "socket" );
        exit( EXIT_FAILURE );
    }

    memset( &addr, 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );

    //
    // Bind this port number to the socket.
    //
    if( bind( p_socket, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ){
        perror( "bind" );
        exit( EXIT_FAILURE );
    }


    return 0;
}
char * generals::get_hostname_from_general_id(GENERAL_ID ID)
{

    switch (ID)
    {
        case CAESAR:
            return "node-caesar";
        case BRUTUS:
            return "node-brutus";
        case POMPUS:
            return "node-pompus";
        case OPERACHORUS:
            return "node-operachorus";
    }
}

int generals::initiate_snapshot()
{
  
    my_state[0] = my_units.catapults;
    my_state[1] = my_units.archers;
    my_state[2] = my_units.cavalry;
    my_state[3] = my_units.spearmen;
    my_state[4] = my_units.infantry;

    memset(&snapshot_global_units,0,sizeof(snapshot_global_units));
    memset(&all_channels,0,sizeof(army_units)*6);
  //  printf("\nSNAPSHOT: number of units caeser has");
    //print_number_of_troops(&my_units);
    process_snapshot_message_general(general_id,my_units);
    int i=(int)general_id;


    snapshot_marker =MARKER_SET;
    MESSAGE *status_msg;
    status_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!status_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    memset(status_msg,0,sizeof(MESSAGE));
    status_msg->message_type=SNAPSHOT_INITIATED;
    status_msg->sender_id=general_id;
    status_msg->observer_id=general_id;
    status_msg->marker=1;

    for(int i=0;i<4;i++)
    {
        if(i!= general_id)
            send_message_udp(GENERAL_ID(i),(char*)status_msg);
    }

}
int generals::process_snapshot_message_general(GENERAL_ID sender_id,army_units my_units)
{
     
     memcpy(&snapshot_generals[sender_id],&my_units,sizeof(army_units));
     snapshot_global_units.catapults +=snapshot_generals[sender_id].catapults;
     snapshot_global_units.archers +=snapshot_generals[sender_id].archers;
     snapshot_global_units.cavalry +=snapshot_generals[sender_id].cavalry;
     snapshot_global_units.spearmen +=snapshot_generals[sender_id].spearmen;
     snapshot_global_units.infantry +=snapshot_generals[sender_id].infantry;

     int ret;
     ret = global_snapshot();
      
}




int generals::process_snapshot_initiated_msg(GENERAL_ID observer_process)
{
    int ret;
    if(snapshot_marker ==MARKER_SET)
    {
        printf("\n I have already received the snap shot initiated msg.. ignoring...");
        return 1;
    }
    snapshot_marker =MARKER_SET;
    observar_general = observer_process;
    ret = send_own_state(observer_process);
    MESSAGE *status_msg;
    status_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!status_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    memset(status_msg,0,sizeof(MESSAGE));
    status_msg->message_type=SNAPSHOT_INITIATED;
    status_msg->sender_id=general_id;
    status_msg->observer_id=observer_process;
    status_msg->marker=MARKER_SET;


    for(int i=0;i<4;i++)
    {
        if(i!= general_id && i!= observer_process)
            send_message_udp(GENERAL_ID(i),(char*)status_msg);
    }

    //Send own state to the observer process and send the marker to other processes

}
int generals::global_snapshot()
{
    int i;
    
    if((snapshot_global_units.catapults ==total_global_units.catapults) &&
        (snapshot_global_units.archers == total_global_units.archers) &&
        (snapshot_global_units.cavalry == total_global_units.cavalry) &&
        (snapshot_global_units.spearmen == total_global_units.spearmen) &&
        (snapshot_global_units.infantry == total_global_units.infantry))
    {
        printf("\n\n SNAPSHOT: is complete!\n\n");
        printf("\n The number of troops in the channel and generals is equal to the initial troops");
       
        print_number_of_troops(&snapshot_global_units);

            printf("\n The state for generals is:\n");
            printf("\n%-10s%-10s%-10s%-10s%-10s", "Generals:","Caesar","Pompus","Brutus","Operchorus");
            printf("\n%-10s", "Catapults:");
        for(i=0;i<4;i++)
        {   
            printf("%-10d",snapshot_generals[i].catapults);
        }
        
        printf("\n%-10s","Archers:");
        for(i=0;i<4;i++)
        {   
            printf("%-10d",snapshot_generals[i].archers);
        }
        
        printf("\n%-10s", "Cavalry:");
        for(i=0;i<4;i++)
        {   
            printf("%-10d",snapshot_generals[i].cavalry);
        }
        printf("\n%-10s","Spearmen:");
        for(i=0;i<4;i++)
        {   
            printf("%-10d",snapshot_generals[i].spearmen);
        }
        
        printf("\n%-10s","Infantry:");
        for(i=0;i<4;i++)
        {   
            printf("%-10d",snapshot_generals[i].infantry);
        }


        printf("\n \nThe state for channels is");
        printf("\n%-10s%-3s%-3s%-3s%-3s%-3s%-3s", "Channels:","1","2","3","4","5","6");
        printf("\n%-10s", "Catapults:");
        for(i=0;i<6;i++)
        {   
            printf("%-3d",all_channels[i].catapults);
        }
        
        printf("\n%-10s","Archers:");
        for(i=0;i<6;i++)
        {   
            printf("%-3d",all_channels[i].archers);
        }
        
        printf("\n%-10s", "Cavalry:");
        for(i=0;i<6;i++)
        {   
            printf("%-3d",all_channels[i].cavalry);
        }
        printf("\n%-10s","Spearmen:");
        for(i=0;i<6;i++)
        {   
            printf("%-3d",all_channels[i].spearmen);
        }
        
        printf("\n%-10s","Infantry:");
        for(i=0;i<6;i++)
        {   
            printf("%-3d",all_channels[i].infantry);
        }
        /*
         for(i=0;i<6;i++)
        {   
            printf("\n The troops for channel %d  is:", i);
            print_number_of_troops(&all_channels[i]);
        }*/
    }
    else
        //printf("\n Snapshot not complete");
    return 0;
        
}


int generals::process_snapshot_message_channel(CHANNEL_ID chan_id,UNIT_TYPES type,int number_of_units)
{
   
   
  // printf("\n SNapshot receieved %d of type %d on channel %d",number_of_units,type,chan_id);
   if(type == CATAPULTS)
   {
       all_channels[chan_id].catapults += number_of_units;
        snapshot_global_units.catapults += number_of_units;
   }
    if(type == ARCHERS)
   {
       all_channels[chan_id].archers+=number_of_units;
        snapshot_global_units.archers +=number_of_units;
   }
    if(type == CAVALRY)
   {
       all_channels[chan_id].cavalry +=number_of_units;
        snapshot_global_units.cavalry +=number_of_units;
   }
    if(type == SPEARMEN)
   {
       all_channels[chan_id].spearmen+=number_of_units;
        snapshot_global_units.spearmen +=number_of_units;
   }
    if(type == INFANTRY)
   {
       all_channels[chan_id].infantry+=number_of_units;
        snapshot_global_units.infantry +=number_of_units;
   }
   int ret;
     ret = global_snapshot();
}


int generals::send_own_state(GENERAL_ID observer_process)
{
    char * msg;
    int ret;
    msg= (char *)malloc(sizeof(MESSAGE));
    memset(msg,0,sizeof(MESSAGE));
    MESSAGE *send_unit_msg;
    send_unit_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!send_unit_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }

    //printf("\n My state is for the snapshot");
    print_number_of_troops(&my_units);
    memset(send_unit_msg,0,sizeof(MESSAGE));
    send_unit_msg->message_type=SNAPSHOT_STATE_GENERAL;
    send_unit_msg->sender_id=general_id;
    send_unit_msg->my_units.catapults = my_units.catapults;
    send_unit_msg->my_units.archers =my_units.archers;
    send_unit_msg->my_units.cavalry =my_units.cavalry;
    send_unit_msg->my_units.spearmen = my_units.spearmen;
    send_unit_msg->my_units.infantry = my_units.infantry;
    send_unit_msg->marker=MARKER_SET;

    memcpy(msg,send_unit_msg,sizeof(MESSAGE));
    ret=send_message_udp(observer_process,(char*)send_unit_msg);
    //printf("\nSent my snapshot state to CAESAR");
    return 0;

}

int generals::process_msg_sent_before_snapshot(GENERAL_ID sender,UNIT_TYPES troop_type, 
int num_of_troops)
{
    int ret;
    MESSAGE *status_msg;
    status_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!status_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    CHANNEL_ID chan_id = get_channel_id(sender, general_id);
    memset(status_msg,0,sizeof(MESSAGE));
    status_msg->message_type=SNAPSHOT_STATE_CHANNEL;
    status_msg->sender_id=general_id;
    status_msg->channel_num=chan_id;
    status_msg->troop_type =troop_type;
    status_msg->num_of_troops = num_of_troops;
    ret=send_message_udp(observar_general,(char*)status_msg);


}

CHANNEL_ID generals::get_channel_id(GENERAL_ID sender, GENERAL_ID receiver)
{
    if(((sender ==  CAESAR) && (receiver == POMPUS)) ||
        ((receiver ==  CAESAR) && (sender == POMPUS)))
    {
        return CAESAR_POMPUS;

    }
    
    if(((sender ==  CAESAR) && (receiver == BRUTUS)) ||
        ((receiver ==  CAESAR) && (sender == BRUTUS)))
    {
        return CAESAR_BRUTUS;

    }
    if(((sender ==  CAESAR) && (receiver == OPERACHORUS)) ||
        ((receiver ==  CAESAR) && (sender == OPERACHORUS)))
    {
        return CAESAR_OPERACHORUS;

    }
    if(((sender ==  POMPUS) && (receiver == BRUTUS)) ||
        ((receiver ==  POMPUS) && (sender == BRUTUS)))
    {
        return POMPUS_BRUTUS;

    }
    
    if(((sender ==  POMPUS) && (receiver == OPERACHORUS)) ||
        ((receiver ==  POMPUS) && (sender == OPERACHORUS)))
    {
        return POMPUS_OPERACHORUS;

    }

    if(((sender ==  BRUTUS) && (receiver == OPERACHORUS)) ||
        ((receiver ==  BRUTUS) && (sender == OPERACHORUS)))
    {
        return BRUTUS_OPERACHORUS;

    }
}
int generals::send_message_udp(GENERAL_ID ID, char *msg)
{
    struct hostent *dest;
    MESSAGE *test;
    int n=0;
    int dest_len;
    char *dest_name = NULL;
    struct sockaddr_in dest_addr;
    memset( &dest_addr, 0, sizeof( addr ) );
    char buffer[MAX_MESSAGE_SIZE];
    dest_name = (char *)malloc(sizeof(buffer));
    test =(MESSAGE*)msg;
    dest_name = get_hostname_from_general_id(ID);
    for(int i =0;i<4;i++)
        test->vector_clock[i]=my_vector_clock[i];
     test->marker=snapshot_marker;
    dest = gethostbyname(dest_name);
    memset( &dest_addr, 0, sizeof( dest_addr ) );
    dest_addr.sin_family = AF_INET;
    bcopy((char *)dest->h_addr,
            (char *)&dest_addr.sin_addr.s_addr, dest->h_length);
    dest_addr.sin_port = htons(generals::p_port);
    dest_len = sizeof(dest_addr);

    memset(buffer,0 ,sizeof(buffer));
    if(!msg)
    {
        printf("Message is empty");
        return 1;
    }
    memcpy(buffer,msg,sizeof(buffer));

    increment_vector_clock();
    n = sendto(generals::p_socket,msg, sizeof(MESSAGE), 0, (struct sockaddr*)&dest_addr,dest_len);


}

int generals::create_check_troop_status_message(char *msg_buffer)
{

    if(!msg_buffer)
    {
        printf("Message buffer empty\n");
        return 1;
    }

    MESSAGE *status_msg;
    status_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!status_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    memset(status_msg,0,sizeof(MESSAGE));
    status_msg->message_type=SEND_UNIT_COUNT;
    status_msg->sender_id=general_id;
    memcpy(msg_buffer,status_msg,sizeof(msg_buffer));

}

int generals::create_send_troop_message(char *msg_buffer,GENERAL_ID send_msg_to, GENERAL_ID send_troops_to,
                                          UNIT_TYPES type,int num_of_units)
{

    if(!msg_buffer)
    {
        printf("Message buffer empty\n");
        return 1;
    }

    MESSAGE *status_msg;
    status_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!status_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    memset(status_msg,0,sizeof(MESSAGE));

    status_msg->message_type=SEND_UNITS;
    status_msg->sender_id=general_id;
    status_msg->send_to = send_troops_to;
    status_msg->troop_type = type;
    status_msg->num_of_troops = num_of_units;
    memcpy(msg_buffer,status_msg,sizeof(MESSAGE));
    //if(status_msg)
    //free(status_msg);

}
int generals::increment_vector_clock()
{
    my_vector_clock[general_id]++;
    //printf("\n Incremented clock [%d]", my_vector_clock[general_id]);
}
int generals::update_vector_clock(int *received_clock)
{ 
    //update my own clock
    my_vector_clock[general_id]++;
    //printf("\n Updating clock:received value:");
    for(int i=0;i<4;i++)
    {
        //printf("[%d]",received_clock[i]);
        if(received_clock[i] >  my_vector_clock[i])
        {
            my_vector_clock[i] = received_clock[i];
        }
    }
    //printf("\n Updated Value of clock"); 
    /*for(int i=0;i<4;i++)
    {
        printf("[%d]",my_vector_clock[i]);
    }*/

}

int generals::initialize_process()
{
    char node_name[104];
    gethostname(node_name, sizeof(node_name));
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\n Process Running on node %s",node_name);

    //Initialize vector clocks to zero.
    memset(my_vector_clock,0,sizeof(my_vector_clock));
    snapshot_marker =0;
    memset(&g_state,0,sizeof(g_state));

    //Initialize the troop numbers
    srand(getpid());
    my_units.catapults = rand() %100 +1;
    my_units.archers = rand() %100 +1;
    my_units.cavalry = rand() %100 +1;
    my_units.spearmen = rand() %100 +1;
    my_units.infantry = rand() %100 +1;

    if(strstr(node_name, "node-caesar" ))
        general_id = CAESAR;

    if(strstr(node_name, "node-pompus" ))
        general_id = POMPUS;

    if(strstr(node_name, "node-operachorus" ))
        general_id = OPERACHORUS;

    if(strstr(node_name, "node-brutus" ))
        general_id = BRUTUS;

    printf("\n My node name is %s, ID=%d",get_hostname_from_general_id(general_id),general_id);
    printf("\n My troop information is as follows\n Catapults:%d \n Archers =%d\n cavalry =%d \
            \n spearmen =%d \n infantry =%d",my_units.catapults,my_units.archers, my_units.cavalry
            ,my_units.spearmen,my_units.infantry );

    return 0;
}
int generals::send_unit_count_to_caesar()
{
    char * msg;
    int ret;
    msg= (char *)malloc(sizeof(MESSAGE));
    memset(msg,0,sizeof(MESSAGE));
    MESSAGE *send_unit_msg;
    send_unit_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!send_unit_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    memset(send_unit_msg,0,sizeof(MESSAGE));
    send_unit_msg->message_type=RECEIVE_UNIT_COUNT;
    send_unit_msg->sender_id=general_id;
    send_unit_msg->my_units.catapults = my_units.catapults;
    send_unit_msg->my_units.archers =my_units.archers;
    send_unit_msg->my_units.cavalry =my_units.cavalry;
    send_unit_msg->my_units.spearmen = my_units.spearmen;
    send_unit_msg->my_units.infantry = my_units.infantry;

    //printf("\nSending the following message to Caesar\n message_type =%d, \n sender id =%d", send_unit_msg->message_type,
    //send_unit_msg->sender_id); 

    memcpy(msg,send_unit_msg,sizeof(MESSAGE));
    ret=send_message_udp(CAESAR,(char*)send_unit_msg);
    //printf("\nSent my unit count to caser");
    //if(msg)
    //free(msg);
    //  if(send_unit_msg)
    //  free(send_unit_msg);

}
//Take the sum of troops across all generals including caeser
int generals::set_total_number_of_troops()
{
    printf("\nTotal troops");
    memcpy(&all_generals[0],&my_units,sizeof(my_units));

    total_global_units.catapults=all_generals[0].catapults + all_generals[1].catapults + all_generals[2].catapults + all_generals[3].catapults;

    total_global_units.archers=all_generals[0].archers + all_generals[1].archers + all_generals[2].archers + all_generals[3].archers;

    total_global_units.cavalry=all_generals[0].cavalry  + all_generals[1].cavalry + all_generals[2].cavalry + all_generals[3].cavalry;

    total_global_units.spearmen=all_generals[0].spearmen + all_generals[1].spearmen + all_generals[2].spearmen + all_generals[3].spearmen;

    total_global_units.infantry=all_generals[0].infantry  + all_generals[1].infantry + all_generals[2].infantry + all_generals[3].infantry;
    print_number_of_troops(&total_global_units);
}

int generals::print_number_of_troops(army_units *units)
{
    printf("\n catapults:%d",units->catapults);
    printf("\n archers:%d",units->archers);
    printf("\n cavalry:%d",units->cavalry);
    printf("\n spearmen:%d",units->spearmen);
    printf("\n infantry:%d",units->infantry);
}

//Find the distribution of troops
int generals::redistribute_troops()
{
    UNIT_TYPES unit_type;
    int i,j;
    int ret;
    int troop[5][4];
    //Do it for a each type of unit
    //Copy the contents of all generals into a 2-D matrix

    for(i=0;i<5;i++)
    {
        for(j=0;j<4;j++)
        {
            if(i==0)
            { 
                troop[i][j]=all_generals[j].catapults;
            }
            if(i==1)
            { 
                troop[i][j]=all_generals[j].archers;
            }
            if(i==2)
            { 
                troop[i][j]=all_generals[j].cavalry;
            }

            if(i==3)
            { 
                troop[i][j]=all_generals[j].spearmen;
            }
            if(i==4)
            { 
                troop[i][j]=all_generals[j].infantry;
            }
        }
    }
    //TODO Change i<5
    for(i=0;i<5;i++)
    {
        //printf("\n Eqalizing unit type %d",i);
        ret=eqaulize_unit(troop[i],UNIT_TYPES(i));
    }

}

int generals::eqaulize_unit(int units[],UNIT_TYPES unit_type)
{
    //Find which type
    int avg_unit;
    int units_surplus[4];
    int units_deficit[4];
    int redistribution[4][4];
    int i,j,ret;
    int more_than_avg,give_away;
    //printf("\n\n Recieved the following unit for each general ");
    for(i=0;i<4;i++)
    {
      //  printf("\nGeneral %s , has %d units of type %d",get_hostname_from_general_id(GENERAL_ID(i)),units[i],unit_type);
    }

    if(unit_type == CATAPULTS)
    {
        avg_unit =  total_global_units.catapults/4;
       // printf("\n Average of catapults is %d",avg_unit);

    }

    if(unit_type == SPEARMEN)
    {
        avg_unit =  total_global_units.spearmen/4;
        //printf("\n Average of spearmen is %d",avg_unit);
    }
    if(unit_type == INFANTRY)
    {
        avg_unit =  total_global_units.infantry/4;
        //printf("\n Average of infantry is %d",avg_unit);
    }
    if(unit_type == CAVALRY)
    {
        avg_unit =  total_global_units.cavalry/4;
        //printf("\n Average of cavalry is %d",avg_unit);
    }

    if(unit_type == ARCHERS)
    {
        avg_unit =  total_global_units.archers/4;
   //     printf("\n Average of archers is %d",avg_unit);
    }

    //Calculate surplus and deficit with each general
    for( i=0;i<4;i++)
    {
        units_surplus[i]=0;
        units_deficit[i]=0;
        if(units[i] > avg_unit)
        {
            units_surplus[i]=units[i]-avg_unit;
            //printf("\n General %s,has a surplus of units by %d",get_hostname_from_general_id(GENERAL_ID(i)),units_surplus[i]);
        }
        else
        {
            units_deficit[i]=avg_unit-units[i];
            //printf("\n General %s,has a deficit of units by %d",get_hostname_from_general_id(GENERAL_ID(i)),units_deficit[i]);
        }
    }
    for(i=0;i<4;i++)
    {

        for(j=0;j<4;j++)
        {
            redistribution[i][j]=0;
        }
    }
    for(i=0;i<4;i++)
    {
        if(units_surplus[i]==0) //General i has nothing to give, move on
        {   
            //printf("\n no Unit surplus for %d,%d",i,units_surplus[i]);
            continue;
        }


        for(j=0;j<4;j++)
        {
                        while(units_surplus[i]>0 && units_deficit[j]>0)
            {
                //printf("\n Redistributing...");
                units_surplus[i]--;
                units_deficit[j]--;
                redistribution[i][j]++;
            }

        }
    }
    for(i=0;i<4;i++)
    {
        if(units_surplus[i] > 1)
        {

            give_away = units_surplus[i]-1;
            //printf("\n Process %s, has %d, more than avg, will give away %d",
                  //  get_hostname_from_general_id(GENERAL_ID(i)),units_surplus[i],give_away);

            for(j=0;j<4;j++)
            {
                if(i!=j && (units_surplus[i]>1)) 
                {
                    if(give_away >0)
                    {
                      //  printf("\n Giving to process %s",get_hostname_from_general_id(GENERAL_ID(j)));
                        redistribution[i][j]++;
                        units_surplus[j]++;
                        units_surplus[i]--;
                        give_away --;
                    }
                }
            }
        }
    }

    //printf("\nRedistribution of units type %d complete, the final outcome is :",unit_type);
    for(i=0;i<4;i++)
    {
        //printf("\n");  
        for(j=0;j<4;j++)
        {
            //printf("%d,",redistribution[i][j]);
     //       fflush(stdout);
        }
    }
    char * msg;
    msg= (char *)malloc(sizeof(MESSAGE));
    for(i=0;i<4;i++)
    {
        //printf("\n");  
        for(j=0;j<4;j++)
        {
           // printf("%d,",redistribution[i][j]);
            if(redistribution[i][j]>0)
            {
                //printf("\nGeneral %s, will send %d, units of type %d, to general %s" ,
                //get_hostname_from_general_id(GENERAL_ID(i)),redistribution[i][j],unit_type,get_hostname_from_general_id(GENERAL_ID(j)));

                memset(msg,0,sizeof(MESSAGE));

                ret = create_send_troop_message(msg,(GENERAL_ID)(i),(GENERAL_ID)(j),unit_type,
                        redistribution[i][j]);  
                ret=send_message_udp((GENERAL_ID)(i),msg);
            }

        }
    }

}

int generals::update_troop_count(UNIT_TYPES unit_type, int num_units,UPDATE_TROOP type )
{
    if(type ==  REMOVE_TROOPS)
        num_units = num_units*(-1);
    switch(unit_type)
    {
        case  CATAPULTS: my_units.catapults=  my_units.catapults + num_units;
                         break;

        case   ARCHERS: my_units.archers=  my_units.archers + num_units;
                        break;
        case  CAVALRY: my_units.cavalry=  my_units.cavalry + num_units;
                       break;
        case  SPEARMEN: my_units.spearmen=  my_units.spearmen + num_units;
                        break;
        case  INFANTRY: my_units.infantry=  my_units.infantry + num_units;

                        break;
        default:printf("\n Undefined update type received in update troop count");
    }
}

 int generals::send_units_to_general(GENERAL_ID send_to, UNIT_TYPES troop_type,int num_of_troops)
{

    int ret;
    MESSAGE *status_msg;
    status_msg = (MESSAGE*)malloc(sizeof(MESSAGE));
    if(!status_msg)
    {
        printf("Message buffer empty\n");
        return 1;
    }
    memset(status_msg,0,sizeof(MESSAGE));
    status_msg->message_type=RECEIVE_UNITS;
    status_msg->sender_id=general_id;
    //status_msg->send_to = send_troops_to;
    status_msg->troop_type =troop_type;
    status_msg->num_of_troops = num_of_troops;
    //printf("\n Msg type %d:",status_msg->message_type);
    //if(status_msg)
    //free(status_msg);
    //printf("\n Careting message to send to troops:%d",status_msg->message_type); 
    //printf("\n Sending %d troops of type %d to %s",num_of_troops,troop_type,
    //get_hostname_from_general_id(GENERAL_ID(send_to));
    ret=update_troop_count(troop_type,num_of_troops,REMOVE_TROOPS);
    ret=send_message_udp(send_to,(char*)status_msg);


}
 
 int generals::receive_units_from_general(GENERAL_ID from, UNIT_TYPES troop_type,int num_of_troops)
{

    int ret;
    printf("\n Receieved more troops updating my troop count");

    ret=update_troop_count(troop_type,num_of_troops,ADD_TROOPS);
    ret=print_number_of_troops(&my_units); 

}
