#include <embUnit/embUnit.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <LtePhyL2Api.h>

#include "api_storage.h"


char* map_api_file(const char* file, size_t* length)
{
    int fd = -1;
    struct stat f_stat;
    char* addr;

    if ((fd = open(file, O_RDONLY)) == -1) {
        fprintf(stderr, "Cant open API file <./%s>\n", file);
        return 0;;
    }

    if (fstat (fd, &f_stat)) {
        fprintf(stderr, "Cant stat API file <./%s>\n", file);
        close(fd);
        return 0;;
    }

    *length = f_stat.st_size;
    addr = mmap(NULL, *length, PROT_READ, MAP_PRIVATE, fd, 0);

    if (addr == MAP_FAILED){
        fprintf(stderr, "Cant map API file <./%s>\n", file);
        close(fd);
        return 0;;
    }

    return addr;
}

const char* msg_type_to_string(uint8_t t)
{
    switch(t){
    case PHY_TXSTART_REQ:    return "PHY_TXSTART_REQ";
    case PHY_TXSTART_CONF:   return "PHY_TXSTART_CONF";
    case PHY_TXSTART_IND:    return "PHY_TXSTART_IND";
    case PHY_TXSDU_REQ:      return "PHY_TXSDU_REQ";
    case PHY_TXSDU_CONF:     return "PHY_TXSDU_CONF";
    case PHY_TXEND_IND:      return "PHY_TXEND_IND";
    case PHY_RXSTART_REQ:    return "PHY_RXSTART_REQ";
    case PHY_RXSTART_CONF:   return "PHY_RXSTART_CONF";
    case PHY_RXSTART_IND:    return "PHY_RXSTART_IND";
    case PHY_RXSDU_IND:      return "PHY_RXSDU_IND";
    case PHY_RXEND_IND:      return "PHY_RXEND_IND";
    case PHY_INIT_REQ:       return "PHY_INIT_REQ";
    case PHY_INIT_IND:       return "PHY_INIT_IND";
    case PHY_RXSTATUS_IND:   return "PHY_RXSTATUS_IND";
    case PHY_RECONFIG_REQ:   return "PHY_RECONFIG_REQ";
    case PHY_RECONFIG_CNF:   return "PHY_RECONFIG_CNF";
    case PHY_START_REQ:      return "PHY_START_REQ";
    case PHY_START_CONF:     return "PHY_START_CONF";
    case PHY_STOP_REQ:       return "PHY_STOP_REQ";
    case PHY_STOP_CONF:      return "PHY_STOP_CONF";
    case PHY_STOP_IND:       return "PHY_STOP_IND";
    case PHY_TXHIADCIUL_REQ: return "PHY_TXHIADCIUL_REQ";
    case PHY_TXHISDU_REQ:    return "PHY_TXHISDU_REQ";
    case PHY_TXDCIULSDU_REQ: return "PHY_TXDCIULSDU_REQ";
    default:                 return "Unknown";
    }
}


int bindiff(char *s1, char *s2, int len)
{
    int i;
    for (i = 0; i < len; i++) 
        if (s1[i] != s2[i]) {
            printf ("bincmp: %d(%02X %02X %02X) >< %d(%02X %02X %02X) at: %d(0x%X)\n",
                    s1[i], s1[i-1],s1[i],s1[i+1],
                    s2[i], s2[i-1],s2[i],s2[i+1], i, i);
            return i;
        }
    return -1;
}

void * dl_msg_from_coff (int msg_type, unsigned char msg[])
{
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) msg;
    if (! apiLogInfo) 
        return NULL;
    
    if (msg_type != apiLogInfo->Message) 
        return NULL;
    
    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    if (msg_type != genhdr->msgType) 
        return NULL;
    
    return (void*) &genhdr[1];
}

void *alloc_api_storage(size_t apiSize, STORAGE_HEADER **storage)
{
    *storage = calloc (1, sizeof(STORAGE_HEADER)
                       + sizeof(API_LOG_INFO)
                       + sizeof(PGENMSGDESC)
                       + apiSize);

    if (! *storage) {
        return 0;
    }
    
    API_LOG_INFO *apiLog = (API_LOG_INFO *) &(*storage)[1];
    PGENMSGDESC  genMsgDesc = (PGENMSGDESC) &apiLog [1];
    return &genMsgDesc [1];
}

void save_api_fragment(int msg_type, STORAGE_HEADER *storage,
                       const char* file_spec,  size_t msgSize)
{
    FILE *out = fopen(file_spec, "wb");
    if (! out) {
        fprintf(stderr, "Can't open output file: %s\n", file_spec);
        exit(EXIT_FAILURE);        
    }

    API_LOG_INFO *apiLog = (API_LOG_INFO *) &storage[1];
    PGENMSGDESC genMsgDesc = (PGENMSGDESC) &apiLog [1];

    genMsgDesc->msgSpecific = msgSize;
    genMsgDesc->msgType = msg_type;
    genMsgDesc->phyEntityId = 0;
    
    apiLog->PayloadLen = ROUND(genMsgDesc->msgSpecific, 4);
    apiLog->Timestamp = 0;
    apiLog->Message = genMsgDesc->msgType;
    apiLog->Source = 0;
    apiLog->Destination = 0;
    apiLog->PHY_ID = 0;
    apiLog->MAC_ID = 0;

    storage->Version     = 256;
    storage->Flag        = 0;
    storage->MsgNum      = 1;
    storage->PayloadSize = sizeof(API_LOG_INFO) + sizeof(GENMSGDESC) + apiLog->PayloadLen;
    storage->LogStorSize = sizeof(STORAGE_HEADER) + storage->PayloadSize;    

    fwrite(storage, storage->LogStorSize, 1, out);
    
    fclose(out);
}

