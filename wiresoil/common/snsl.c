#include "vdip.h"
#include "snsl.h"

// For `malloc` and `free`
#include <stdlib.h>

char** SNSL_ParseNodeNames(void)
{
    char *psz_data   = VDIP_ReadFile(FILE_NODES);
    uint32 u32_size  = VDIP_FileSize(FILE_NODES),
           u32_index = 0;
           
    //printf("Read: `%s`\n\n", psz_data);
    //printf("Size: `%u`\n\n", (unsigned)u32_size);

    // Count the newlines to count the number
    // of nodes we need to allocate memory for
    uint32 u32_nodes = 0,
           u32_delimiter_width = 1;
    
    for(; u32_index < u32_size; ++u32_index)
    {
        //printf("`%u`: Take care of linux and windows newlines\n", (unsigned)u32_index);
        if(psz_data[u32_index] == '\n' ||
           psz_data[u32_index] == '\r' ||
           (u32_index == u32_size-1 &&
            psz_data[u32_index] != '\n'))
        {
            if(u32_index < u32_size &&
               psz_data[u32_index] == '\n')
            {
               ++u32_index;
               u32_delimiter_width = 2;
            }
            ++u32_nodes;
        }
    }

    //printf("Nodes found: `%u`\n", (unsigned)u32_nodes);
    //printf("Delim width: `%u`\n", (unsigned)u32_delimiter_width);

    // Allocate space - node addresses are fixed
    // at three characters.
    char **psz_nodes = (char**)malloc(sizeof(char*) * (u32_nodes + 1));

    // Null terminate the array so it will be easy to traverse
    psz_nodes[u32_nodes] = '\0';

    // Allocate memory for the filenames. This could have
    // been done in one call, but contiguous memory space
    // may be sparse.
    for(u32_index = 0; u32_index < u32_nodes; ++u32_index)
    {
        psz_nodes[u32_index] = (char*)malloc(sizeof(char) * MAX_NODE_NAME_LEN);
    }
    
    // Fill the array
    int u8_hex;
    uint8 u8_index,
          u8_node = 0;
    char  sz_hex[2];
    for(u32_index = 0; u32_index < u32_size; u32_index += u32_delimiter_width)
    {
        for(u8_index = 0; u8_index < 3; ++u8_index)
        {
            sz_hex[0] = (char)psz_data[u32_index++];
            sz_hex[1] = (char)psz_data[u32_index++];
            sscanf(sz_hex, "%x", &u8_hex);
            //printf("0,1,h:`%u`,`%u` `%c`, `0x%X`, `%c`,`%d`,`0x%X`\n",
            //    u8_node, u8_index, sz_hex[0], sz_hex[1], u8_hex, u8_hex, u8_hex);
            psz_nodes[u8_node][u8_index] = u8_hex;
        }
        psz_nodes[u8_node++][u8_index] = '\0';
    }
    free(psz_data);
    
    return psz_nodes;
}
