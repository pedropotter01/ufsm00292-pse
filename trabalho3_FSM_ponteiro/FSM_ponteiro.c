#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03

typedef enum
{
    ST_STX = 0,
    ST_QTD,
    ST_DATA,
    ST_CHECKSUM,
    ST_ETX,
    ST_END,
    ST_FAIL
} State;

typedef struct
{
    State status;
    uint8_t count;
    uint8_t data[256];
    uint8_t checksum;
    uint8_t dataIndex;
    bool (*stateFunc)(struct FSM *, uint8_t);
} FSM;

bool fsm_stx(FSM *fsm, uint8_t byte);
bool fsm_qtd(FSM *fsm, uint8_t byte);
bool fsm_data(FSM *fsm, uint8_t byte);
bool fsm_checksum(FSM *fsm, uint8_t byte);
bool fsm_etx(FSM *fsm, uint8_t byte);

bool (*stateTable[])(FSM *, uint8_t) = {fsm_stx, fsm_qtd, fsm_data, fsm_checksum, fsm_etx};

void fsm_initialize(FSM *fsm)
{
    fsm->status = ST_STX;
    fsm->count = 0;
    fsm->checksum = 0;
    fsm->dataIndex = 0;
    fsm->stateFunc = stateTable[fsm->status];
}

bool fsm_process_received_data(FSM *fsm, uint8_t byte)
{
    if (fsm->status < sizeof(stateTable) / sizeof(stateTable[0]))
    {
        bool completed = fsm->stateFunc(fsm, byte);
        if (fsm->status < sizeof(stateTable) / sizeof(stateTable[0]))
        {
            fsm->stateFunc = stateTable[fsm->status];
        }
        return completed;
    }
    return false;
}

// state functions

bool fsm_stx(FSM *fsm, uint8_t byte)
{
    if (byte == STX)
    {
        fsm->status = ST_QTD;
    }
    return false;
}

bool fsm_qtd(FSM *fsm, uint8_t byte)
{
    fsm->count = byte;
    fsm->dataIndex = 0;
    fsm->status = ST_DATA;
    return false;
}

bool fsm_data(FSM *fsm, uint8_t byte)
{
    if (fsm->dataIndex < fsm->count)
    {
        fsm->data[fsm->dataIndex] = byte;
        fsm->dataIndex++;

        if (fsm->dataIndex == fsm->count)
        {
            fsm->status = ST_CHECKSUM;
        }
    }
    return false;
}

bool fsm_checksum(FSM *fsm, uint8_t byte)
{
    fsm->checksum = byte;
    fsm->status = ST_ETX;
    return false;
}

bool fsm_etx(FSM *fsm, uint8_t byte)
{
    if (byte == ETX)
    {
        fsm->status = ST_END;
        return true;
    }
    else
    {
        fsm->status = ST_FAIL;
    }
    return false;
}

void test()
{
    FSM fsm;
    fsm_initialize(&fsm);

    uint8_t transmission[] = {STX, 0x03, '1', '2', '3', 0x05, ETX};
    bool completed = false;

    for (int i = 0; i < sizeof(transmission); i++)
    {
        if (fsm_process_received_data(&fsm, transmission[i]))
        {
            completed = true;
            break;
        }
    }

    (completed && fsm.status == ST_END) ? printf("Teste passou, FSM funcionando.\n") : printf("Teste falhou.\n");
}

int main()
{
    test();
    return 0;
}
