
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

    
char *out_dir = "out", *prefix, *api_file;
int verbose;

STORAGE_HEADER *apiplayer_storage = NULL;

void usage(const char *prog)
{
    printf("Usage: %s [OPTION] [FILE]\n"
           "Split TT-Box API file by messages into `out' directory.\n"
           "\n"
           "\t-o, output directory\n"
           "\t-p, split file prefix\n"
           "\t-v, verbose\n"
           "\t-h, this help\n"
           , prog);
}

int parse_args(int argc, char *argv[])
{
    int flags, opt;
    flags = 0;
    while ((opt = getopt(argc, argv, "o:p:v")) != -1) {
        switch (opt) {
        case 'v':
            verbose = 1;
            break;
        case 'o':
            out_dir = strdup(optarg);
            break;
        case 'p':
            prefix = strdup(optarg);
            break;
        default: /* '?' */
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected API file after options\n");
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    api_file = strdup(argv[optind]);

    return argc;
}

int dir_exist (char *dir_name)
{
    struct stat d_stat;
    int ret;
    ret = stat (dir_name, &d_stat);
  
    return ret == 0 && S_ISDIR(d_stat.st_mode);
}

char* map_api_file(const char* file, size_t* length)
{
    int fd = -1;
    int ret;
    struct stat f_stat;
    char* addr;
    
    if ((fd = open(file, O_RDONLY)) == -1) {
        fprintf(stderr, "Cant open API file <./%s>\n", file);
        exit(EXIT_FAILURE);                
    }

    if (fstat (fd, &f_stat)) {
        fprintf(stderr, "Cant stat API file <./%s>\n", file);
        close(fd);
        exit(EXIT_FAILURE);                        
    }
    
    *length = f_stat.st_size;
    addr = mmap(NULL, *length, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if (addr == MAP_FAILED){
        fprintf(stderr, "Cant map API file <./%s>\n", file);
        close(fd);
        exit(EXIT_FAILURE);                        
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

void save_api_to_storage(STORAGE_HEADER *storage,
                         const char* file_spec, const char* buff, size_t lenght)
{
    STORAGE_HEADER shdr;
    
    FILE *out = fopen(file_spec, "wb");
    if (! out) {
        fprintf(stderr, "Can't open output file: %s\n", file_spec);
        exit(EXIT_FAILURE);        
    }


    shdr.Version = storage->Version;    
    shdr.Flag=storage->Flag;    
    shdr.MsgNum = 1;
    
    shdr.PayloadSize = lenght;    
    shdr.LogStorSize = lenght + sizeof(STORAGE_HEADER);

    if (! storage){
        fprintf(stderr, "Cant locate storage header file.\n");
        fclose(out);
        exit(EXIT_FAILURE);                        
    }
    
    fwrite(&shdr, sizeof(STORAGE_HEADER), 1, out);
    
    fwrite(buff, lenght, 1, out);

    fclose(out);
}

int main(int argc, char *argv[])
{
    char *addr, *pmsg;
    size_t length;
    int idx;
    
    parse_args(argc, argv);

    if (verbose) {
        printf("Processing API file <%s>\n\tinto messages %s/%s*\n",
               api_file, out_dir, prefix);
    }

    if (!dir_exist(out_dir)) {
        fprintf(stderr, "Output directory <./%s> doesn't exist; "
                "Please create is first.\n");
        exit(EXIT_FAILURE);
    }

    addr = map_api_file(api_file, &length);

    apiplayer_storage = (STORAGE_HEADER *) addr;

    if (verbose) {
    printf("STORAGE_HEADER:\n"
           "pHeader->Version     = %d\n"
           "pHeader->Flag        = %x\n"
           "pHeader->MsgNum      = %d\n"
           "pHeader->PayloadSize = %d\n"
           "pHeader->LogStorSize = %d\n"
           "\n",
           apiplayer_storage->Version,
           apiplayer_storage->Flag,
           apiplayer_storage->MsgNum,
           apiplayer_storage->PayloadSize,
           apiplayer_storage->LogStorSize);
    }

    pmsg = addr + sizeof(STORAGE_HEADER);
    

    size_t API_LOG_INFO_SIZE = sizeof(API_LOG_INFO) + (apiplayer_storage->Version == 768 ? 8 : 0);
    
    for (idx = 0; pmsg < addr + length; ++idx) {
        char file_spec[BUFSIZ];
        API_LOG_INFO *apiInfo = (API_LOG_INFO *) pmsg;
        const char *msgTypeS = msg_type_to_string(apiInfo->Message);
        uint32_t lenght = ROUND(apiInfo->PayloadLen, 4);

        if (verbose) {
            printf("[%d] msgId:%s(%d) size: %d(0x%x) at 0x%x\n",
                   idx,  msgTypeS, apiInfo->Message, apiInfo->PayloadLen,
                   apiInfo->PayloadLen, pmsg - addr);
        }

        sprintf(file_spec, "%s/%s%04d_%s.bin", out_dir, prefix, idx, msgTypeS);

        
        save_api_to_storage(apiplayer_storage, file_spec, pmsg, API_LOG_INFO_SIZE + lenght);
        
        pmsg += API_LOG_INFO_SIZE + lenght;

    }

    printf("OK\n");
    return 0;
}
