#include <Arduino.h>

const byte ROWS = 4;
const byte COLS = 4;

char keysMapping[ROWS][COLS] = {
    {'A', 'B', 'C', 'D'},
    {'E', 'F', 'G', 'H'},
    {'I', 'J', 'K', 'L'},
    {'M', 'N', 'O', 'P'}
};

int keysLastValue[ROWS][COLS] = {
    {HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH}
};

int keysValue[ROWS][COLS] = {
    {HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH}};

unsigned long keysLastDebounce[ROWS][COLS] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

unsigned long debounceDelay = 50;

byte rows[] = {A5, A3, A4, A2};
const int rowCount = sizeof(rows) / sizeof(rows[0]);

byte cols[] = {1, 2, 0, 3};
const int colCount = sizeof(cols) / sizeof(cols[0]);

byte keys[colCount][rowCount];

void setup()
{
    Serial.begin(115200);

    for (int x = 0; x < rowCount; x++)
    {
        pinMode(rows[x], INPUT);
    }

    for (int x = 0; x < colCount; x++)
    {
        pinMode(cols[x], INPUT_PULLUP);
    }
}

void readMatrix()
{
    for (int colIndex = 0; colIndex < colCount; colIndex++)
    {
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);

        for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
            byte rowCol = rows[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            keys[colIndex][rowIndex] = digitalRead(rowCol);
            pinMode(rowCol, INPUT);
        }

        pinMode(curCol, INPUT);
    }
}

void printMatrix()
{
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        if (rowIndex < 10) {
            Serial.print(F("0"));
        }

        Serial.print(rowIndex);
        Serial.print(F(": "));

        for (int colIndex = 0; colIndex < colCount; colIndex++)
        {
            Serial.print(keys[colIndex][rowIndex]);
            if (colIndex < colCount) {
                Serial.print(F(", "));
            }
        }
        Serial.println("");
    }
    Serial.println("");
}

void printDirect()
{
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (int colIndex = 0; colIndex < colCount; colIndex++)
        {
            if (keys[colIndex][rowIndex] == LOW)
            {
                if (keys[colIndex][rowIndex] != keysLastValue[colIndex][rowIndex])
                {
                    // reset the debouncing timer
                    keysLastDebounce[colIndex][rowIndex] = millis();
                    keysLastValue[colIndex][rowIndex] = keys[colIndex][rowIndex];
                }

                if ((millis() - keysLastDebounce[colIndex][rowIndex]) > debounceDelay)
                {
                    if (keys[colIndex][rowIndex] != keysValue[colIndex][rowIndex])
                    {
                        keysValue[colIndex][rowIndex] = keys[colIndex][rowIndex];

                        if (keysValue[colIndex][rowIndex] == LOW)
                        {
                            Serial.print("Col: ");
                            Serial.println(colIndex);
                            Serial.print("Row: ");
                            Serial.println(rowIndex);
                            Serial.println("");
                            Serial.println(keysMapping[colIndex][rowIndex]);
                        }
                    }
                }
            } else {
                keysLastValue[colIndex][rowIndex] = keys[colIndex][rowIndex];
                keysValue[colIndex][rowIndex] = keys[colIndex][rowIndex];
            }
        }
    }
}

void loop()
{
    readMatrix();
    if (Serial.read() == '!') {
        printMatrix();
    }
    printDirect();
}