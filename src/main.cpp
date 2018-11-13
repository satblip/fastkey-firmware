#include <Arduino.h>

const byte ROWS = 4;
const byte COLS = 4;

char keysMapping[ROWS][COLS] = {
    {'P', 'L', 'H', 'D'},
    {'O', 'K', 'G', 'C'},
    {'N', 'J', 'F', 'B'},
    {'M', 'I', 'E', 'A'}};

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

byte cols[] = {A5, A4, A3, A2};
const int rowCount = sizeof(cols) / sizeof(cols[0]);

byte rows[] = {1, 0, 2, 3};
const int colCount = sizeof(rows) / sizeof(rows[0]);

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
        pinMode(cols[x], INPUT);
    }
}

void readMatrix()
{
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        byte curRow = rows[rowIndex];
        pinMode(curRow, OUTPUT);
        digitalWrite(curRow, LOW);

#ifdef DEBUG_MATRIX
        Serial.print("row ");
        Serial.print(rowIndex+1);
        Serial.print("|");
        Serial.println(curRow);
#endif

        for (int colIndex = 0; colIndex < colCount; colIndex++)
        {
            byte curCol = cols[colIndex];
            pinMode(curCol, INPUT_PULLUP);
            keys[rowIndex][colIndex] = digitalRead(curCol);
            pinMode(curCol, INPUT);

#ifdef DEBUG_MATRIX
            Serial.print("col ");
            Serial.print(colIndex+1);
            Serial.print("|");
            Serial.print(curCol);
            Serial.print("|");
            Serial.println(digitalRead(curCol));
#endif
        }

        pinMode(curRow, INPUT_PULLUP);
    }
#ifdef DEBUG_MATRIX
    delay(5000);
#endif
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
            Serial.print(keys[rowIndex][colIndex]);
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
            if (keys[rowIndex][colIndex] == LOW)
            {
                if (keys[rowIndex][colIndex] != keysLastValue[rowIndex][colIndex])
                {
                    // reset the debouncing timer
                    keysLastDebounce[rowIndex][colIndex] = millis();
                    keysLastValue[rowIndex][colIndex] = keys[rowIndex][colIndex];
                }

                if ((millis() - keysLastDebounce[rowIndex][colIndex]) > debounceDelay)
                {
                    if (keys[rowIndex][colIndex] != keysValue[rowIndex][colIndex])
                    {
                        keysValue[rowIndex][colIndex] = keys[rowIndex][colIndex];

                        if (keysValue[rowIndex][colIndex] == LOW)
                        {
#ifdef DEBUG_KEY
                            Serial.print("Col: ");
                            Serial.println(colIndex+1);
                            Serial.print("Row: ");
                            Serial.println(rowIndex+1);
                            Serial.println("");
#endif
                            Serial.println(keysMapping[rowIndex][colIndex]);
                        }
                    }
                }
            } else {
                keysLastValue[rowIndex][colIndex] = keys[rowIndex][colIndex];
                keysValue[rowIndex][colIndex] = keys[rowIndex][colIndex];
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