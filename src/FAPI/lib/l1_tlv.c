#include <stdint.h>
#include <stddef.h>

#include <l1_types.h>
#include <l1_tlv.h>

int find_tlv_idx (FAPI_L1_TLV_TAG_e tag, l1_tlv_word_t array[], uint8_t size)
{
    int i;
    for (i = 0; i < size; i++)
        if (array[i].tag == tag)
            return i;
    return -1;
}

l1_tlv_word_t * find_tlv(FAPI_L1_TLV_TAG_e tag, l1_tlv_word_t array[], uint8_t size)
{
    int idx = find_tlv_idx (tag, array, size);
     
    return (idx < 0) ? NULL : &array[idx];
}

