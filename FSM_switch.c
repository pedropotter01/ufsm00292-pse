#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ST_WAIT_STX = 0, ST_READ_COUNT, ST_READ_DATA, ST_READ_CHECKSUM, ST_WAIT_ETX, ST_END, ST_FAIL
} State;

typedef struct {
    State status;
    uint8_t count;
    uint8_t data[256];
    uint8_t checksum;   
    uint8_t index;    
} FSM;

void fsm_initialize(FSM *fsm) {
    fsm->status = ST_WAIT_STX;
    fsm->count = 0;
    fsm->checksum = 0;
    fsm->index = 0;
}

bool fsm_process_received_data(FSM *fsm, uint8_t byte) {
    switch (fsm->status) {
        case ST_WAIT_STX:
            if (byte == 0x02) { // STX
                fsm->status = ST_READ_COUNT;
            }
            break;
        case ST_READ_COUNT:
            fsm->count = byte;
            fsm->status = ST_READ_DATA;
            break;
        case ST_READ_DATA:
            fsm->data[fsm->index++] = byte;
            if (fsm->index == fsm->count) {
                fsm->status = ST_READ_CHECKSUM;
            }
            break;
        case ST_READ_CHECKSUM:
            fsm->checksum = byte;
            fsm->status = ST_WAIT_ETX;
            break;
        case ST_WAIT_ETX:
            if (byte == 0x03) { // ETX
                fsm->status = ST_END;
                return true;
            } else {
                fsm->status = ST_FAIL;
            }
            break;
        case ST_END:
        case ST_FAIL:
            break;
    }
    return false;
}

void FSM_test() {
    FSM fsm;
    fsm_initialize(&fsm);

    uint8_t transmission[] = {0x02, 0x03, '1', '2', '3', 0x05, 0x03};
    bool result = false;

    for (int i = 0; i < sizeof(transmission); i++) {
        result = fsm_process_received_data(&fsm, transmission[i]);
        if (result) {
            break;
        }
    }

    if (result && fsm.status == ST_END) {
        printf("Teste passou, FSM funcionando.\n");
    } else {
        printf("Teste falhou.\n");
    }
}

int main() {
    FSM_test();
    return 0;
}
