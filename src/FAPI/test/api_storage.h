#ifndef _API_STORAGE_H_
#define _API_STORAGE_H_

typedef struct _STORAGE_HEADER_
{
    // THE DATA IS CACHE ALIGNED!!

    uint32_t      Version;        // Log version (for the host application and for the C-CODE)
    uint32_t      Flag;           // Status flags: see: LOGGER_FLAG_
    uint32_t      MsgNum;         // The number of messages written to the log
    uint32_t      PayloadSize;    // The size of data located in the log storage
    uint32_t      LogStorSize;    // The size of log storage
    uint32_t      Res [3];        // Reserved

    // THE DATA IS CACHE ALIGNED!!
}STORAGE_HEADER;

typedef struct _API_LOG_INFO_
{
    uint32_t PayloadLen;  // Just length of payload, it does not include length of header
    uint32_t Timestamp;
    uint32_t Message;
    uint8_t  Source;
    uint8_t  Destination;
    uint8_t  PHY_ID;
    uint8_t  MAC_ID;
    uint8_t  Reserved[8];

}API_LOG_INFO;


#define ROUND(x, dx)  (((x) + ((dx) - 1) ) & ~((dx) - 1))


/** 
 * This function memory map binary file.
 * Memory can't be unmaped and will free on exit only.
 * 
 * @param file   - file to map
 * @param length - len of the file
 * 
 * @return pointer to mapped memory
 */
char* map_api_file(const char* file, size_t* length);

/**
 * msg_type_to_string
 *
 * @param t - PHY message type
 * 
 * @return readable name of message type
 */
const char* msg_type_to_string(uint8_t t);

int bindiff (char *s1, char *s2, int len);
extern void assertImplementationInt(int expected,int actual, long line, const char *file);

#define TEST_ASSERT_EQUAL_BIN(expected,actual,len){                     \
        int at;                                                         \
        if (/* expected && actual && */ ((at = bindiff((char *)expected,(char *)actual, len)) < 0)) {} \
        else {assertImplementationInt( ((char *)expected)[at], ((char*)actual)[at],__LINE__,__FILE__); \
            return;}} while (0)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
    
void * dl_msg_from_coff (int msg_type, unsigned char msg[]);

void *alloc_api_storage(size_t apiSize, STORAGE_HEADER **storage);

void save_api_fragment(int msg_type, STORAGE_HEADER *storage,
                       const char* file_spec,  size_t msgSize);


#endif /* _API_STORAGE_H_ */
