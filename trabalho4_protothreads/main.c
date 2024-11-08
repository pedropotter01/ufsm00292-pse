#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pt.h>

#define STX 0x02
#define ETX 0x03
#define BUFFER_SIZE 100
#define ITERATE 1

typedef struct
{
    char string[BUFFER_SIZE];
    int start;
    int end;
    int full;
} RingBuffer;

void initializeBuffer(RingBuffer *circBuffer)
{
    circBuffer->start = 0;
    circBuffer->end = 0;
    circBuffer->full = 0;
}

int bufferEmpty(RingBuffer *circBuffer)
{
    return (!circBuffer->full && (circBuffer->start == circBuffer->end));
}

int bufferFull(RingBuffer *circBuffer)
{
    return circBuffer->full;
}

void bufferInsert(RingBuffer *circBuffer, char newValue)
{
    if (bufferFull(circBuffer))
        circBuffer->start = (circBuffer->start + 1) % BUFFER_SIZE;

    circBuffer->string[circBuffer->end] = newValue;
    circBuffer->end = (circBuffer->end + 1) % BUFFER_SIZE;
    circBuffer->full = (circBuffer->end == circBuffer->start);
}

char deleteFromBuffer(RingBuffer *circBuffer)
{
    assert(!bufferEmpty(circBuffer));

    char value = circBuffer->string[circBuffer->start];
    circBuffer->start = (circBuffer->start + 1) % BUFFER_SIZE;
    circBuffer->full = 0;

    return value;
}

unsigned char checksum(char *data, int dataLength)
{
    unsigned char chk = 0;
    for (int i = 0; i < dataLength; i++)
        chk ^= data[i];

    return chk;
}

int encode(char *destination, char *source, int dataLength)
{
    if (dataLength > BUFFER_SIZE - 4)
        return -1;

    destination[0] = STX;
    destination[1] = dataLength;
    memcpy(&destination[2], source, dataLength);
    destination[2 + dataLength] = checksum(source, dataLength);
    destination[3 + dataLength] = ETX;
    return 4 + dataLength;
}

int decode(char *source, int dataLength, char *destination)
{
    if (source[0] != STX || source[dataLength - 1] != ETX)
        return -1;

    int qtd = source[1];

    if (qtd + 4 != dataLength)
        return -1;

    if (source[2 + qtd] != checksum(&source[2], qtd))
        return -1;

    memcpy(destination, &source[2], qtd);
    return qtd;
}

void verifyProtocol()
{
    char testData[] = "ABCD";
    char encodedString[256];
    char decodedString[256];

    int encodedStringLength = encode(encodedString, testData, sizeof(testData) - 1);
    assert(encodedStringLength == 8);
    assert(encodedString[0] == STX);
    assert(encodedString[1] == 4);
    assert(encodedString[6] == checksum(testData, 4));
    assert(encodedString[7] == ETX);

    int decodedStringLength = decode(encodedString, encodedStringLength, decodedString);
    assert(decodedStringLength == 4);
    assert(memcmp(decodedString, testData, 4) == 0);

    printf("Testes passaram.\n");
}

static struct pt pt_tx, pt_rx;
RingBuffer txBuffer, rxBuffer;

static PT_THREAD(transmitter_Protothread(struct pt *pt))
{
    PT_BEGIN(pt);

    static char data[] = "test123";
    static char encodedString[256];
    static int encodedStringLength;

    while (1)
    {
        encodedStringLength = encode(encodedString, data, sizeof(data) - 1);
        for (int i = 0; i < encodedStringLength; i++)
        {
            bufferInsert(&txBuffer, encodedString[i]);
        }
        PT_YIELD(pt);
    }

    PT_END(pt);
}

static PT_THREAD(receiver_protothread(struct pt *pt))
{
    PT_BEGIN(pt);

    static char decodedString[256];
    static char receivedBuffer[256];
    static int receivedSize;
    static int decodedStringLength;

    while (1)
    {
        receivedSize = 0;
        while (!bufferEmpty(&txBuffer))
        {
            receivedBuffer[receivedSize++] = deleteFromBuffer(&txBuffer);
            if (receivedBuffer[receivedSize - 1] == ETX)
            {
                break;
            }
        }

        decodedStringLength = decode(receivedBuffer, receivedSize, decodedString);
        if (decodedStringLength > 0)
        {
            printf("Transmissao bem sucedida - %s\n", decodedString);
            bufferInsert(&rxBuffer, 'A');
        }
        else
        {
            printf("Erro na transmissao.\n");
        }
        PT_YIELD(pt);
    }

    PT_END(pt);
}

int main()
{
    PT_INIT(&pt_tx);
    PT_INIT(&pt_rx);
    initializeBuffer(&txBuffer);
    initializeBuffer(&rxBuffer);

    verifyProtocol();

    int index = 0;

    while (index < ITERATE)
    {
        transmitter_Protothread(&pt_tx);
        receiver_protothread(&pt_rx);
        index++;
    }

    return 0;
}
