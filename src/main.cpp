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

byte rows[] = {
  D1,
  D0,
  D18,
  D19
};
const int rowsCount = sizeof(rows) / sizeof(rows[0]);

byte cols[] = {
  A7,
  A6,
  A3,
  A2
};
const int colsCount = sizeof(cols) / sizeof(cols[0]);

byte keys[colsCount][rowsCount];

void setup()
{
    Serial.begin(115200);

    for (int x = 0; x < rowsCount; x++)
    {
        pinMode(rows[x], INPUT);
    }

    for (int x = 0; x < colsCount; x++)
    {
        pinMode(cols[x], INPUT);
    }
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
}

void readMatrix()
{
    for (int rowIndex = 0; rowIndex < rowsCount; rowIndex++)
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

        for (int colIndex = 0; colIndex < colsCount; colIndex++)
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
    for (int rowIndex = 0; rowIndex < rowsCount; rowIndex++)
    {
        if (rowIndex < 10) {
            Serial.print(F("0"));
        }

        Serial.print(rowIndex);
        Serial.print(F(": "));

        for (int colIndex = 0; colIndex < colsCount; colIndex++)
        {
            Serial.print(keys[rowIndex][colIndex]);
            if (colIndex < colsCount) {
                Serial.print(F(", "));
            }
        }
        Serial.println("");
    }
    Serial.println("");
}

void printDirect()
{
    for (int rowIndex = 0; rowIndex < rowsCount; rowIndex++)
    {
        for (int colIndex = 0; colIndex < colsCount; colIndex++)
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
    // digitalWrite(9, HIGH);
    // delay(1000);
    // digitalWrite(9, LOW);
    // delay(1000);
    // digitalWrite(10, HIGH);
    // delay(1000);
    // digitalWrite(10, LOW);
    // delay(1000);
}
