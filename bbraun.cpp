#include "bbraun.h"

Bbraun::Bbraun()
{

}


void Bbraun::create_frame_list_from_byte(unsigned char b){
    switch(b){
        case SOHCHAR:
            m_storestart = true;
            m_storeend = false;
            b_list.push_back(b);
            break;
        case EOTCHAR:
            m_storestart = false;
            m_storeend = true;
            break;
        case ACKCHAR:
            break;
        case NAKCHAR:
            break;
        default:
            if(m_storestart == true && m_storeend == false)
                b_list.push_back(b);
            break;
    }


    // character debuffer
    if(m_storestart == false && m_storeend == true){
        for(int i=0;i<b_list.size();i++){
            unsigned char bchar = b_list[i];
            switch(bchar){
                case ECHAR:
                    if(b_list[i+1] == XCHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = DCHAR;

                    }
                    else if(b_list[i+1] == ECHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = ECHAR;
                    }
                    break;

                case eCHAR:
                    if(b_list[i+1] == xCHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = dCHAR;
                    }
                    else if(b_list[i+1] == eCHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = eCHAR;
                    }
                    break;
            }
        }


        int framelen = b_list.size();

        /*
        if(framelen!=0){
            cvector_vector_type(char) frameBuffer2 = NULL;
            cvector_copy(frameBuffer, frameBuffer2);
            char checksum_buf[5];
            int checksum_rece=0;

            //get away the checksum
            for(int n=0;n<5;n++){
                checksum_buf[n] = frameBuffer2[framelen-1-n];
                cvector_pop_back(frameBuffer2);
            }

            for(int n=0;n<5;n++){
                if(checksum_buf[n]==0)
                    break;
                else
                    checksum_rece += ((int)checksum_buf[n]-48)*(myPow(10,n));
            }
            int checksum_computed = compute_checksum(frameBuffer2);

            printf("\nthe rece_checksum is %d", checksum_rece);
            printf("\nthe calc_checksum is %d", checksum_computed);
            //print_array(frameBuffer2,cvector_size(frameBuffer2));
            if(checksum_rece == checksum_computed){
                printf("\ncorrect checksum");
                ack_flag = true;
                while(!cvector_empty(frameBuffer)){
                    cvector_pop_back(frameBuffer);
                }

            }
        }*/
    }
}


/**
 * with this request the host system initialize the connection to ComDevice
*/
void Bbraun::request_initialize_connection(){
    char bedid[] = "1/1/1";
    char command[] = "ADMIN:ALIVE";
    int len1 = 5;
    int len2 = 11;
    //WriteRequestBuffer(bedid, command, len1, len2);
}

/**
 * with this request the host asks for all actual parameter values of the ComDevice / pumps
 */
void send_get_mem_request(){
    char bedid[] = "SpaceCom9/1/1";
    char command[] = "MEM:GET";
    int len1 = 13;
    int len2 = 7;
    //WriteRequestBuffer(bedid, command, len1, len2);
}
