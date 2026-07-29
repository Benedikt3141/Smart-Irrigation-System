#include "functions.h"

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "pindefinitions.h"

namespace {

// 4 MHz
constexpr uint32_t SD_SPI_FREQUENCY = 4000000UL;

constexpr char SD_TEST_FILE[] = "/SIS_TST.TXT";
constexpr char SD_RENAMED_FILE[] = "/SIS_TST2.TXT";

bool sdCardMounted = false;


// ---------------------------------------------------------
// help functions
// ---------------------------------------------------------

void printTestResult(const char* description, bool successful) {
    Serial.printf(
        "  %-32s %s\n",
        description,
        successful ? "[OK]" : "[FEHLER]"
    );
}


const char* getCardTypeName(uint8_t cardType) {
    switch (cardType) {
        case CARD_MMC:
            return "MMC";

        case CARD_SD:
            return "SDSC";

        case CARD_SDHC:
            return "SDHC/SDXC";

        case CARD_NONE:
            return "Keine Karte";

        default:
            return "Unbekannt";
    }
}


bool writeTextFile(const char* path, const String& content) {
    File file = SD.open(path, FILE_WRITE);

    if (!file) {
        return false;
    }

    const size_t bytesWritten = file.print(content);

    file.flush();
    file.close();

    return bytesWritten == content.length();
}


bool appendTextFile(const char* path, const String& content) {
    File file = SD.open(path, FILE_APPEND);

    if (!file) {
        return false;
    }

    const size_t bytesWritten = file.print(content);

    file.flush();
    file.close();

    return bytesWritten == content.length();
}


bool readTextFile(const char* path, String& content) {
    File file = SD.open(path, FILE_READ);

    if (!file) {
        return false;
    }

    content = "";
    content.reserve(file.size() + 1);

    while (file.available()) {
        const int value = file.read();

        if (value < 0) {
            file.close();
            return false;
        }

        content += static_cast<char>(value);
    }

    file.close();
    return true;
}

}  // namespace


// ---------------------------------------------------------
// SD-Card initialization
// ---------------------------------------------------------

bool initSDCard() {
    Serial.println();
    Serial.println("Initialize SD-Card...");

    sdCardMounted = false;

    pinMode(CS_SD, OUTPUT);
    digitalWrite(CS_SD, HIGH);

    SPI.begin(
        SCL_SPI,
        MISO,
        MOSI,
        CS_SD
    );

    if (!SD.begin(CS_SD, SPI, SD_SPI_FREQUENCY)) {
        Serial.println("[ERROR] SD Mounting failed");
        return false;
    }

    const uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("[ERROR] NO SD-Card detected");
        SD.end();
        return false;
    }

    sdCardMounted = true;

    const uint64_t cardSizeMiB =
        SD.cardSize() / (1024ULL * 1024ULL);

    const uint64_t totalSpaceMiB =
        SD.totalBytes() / (1024ULL * 1024ULL);

    const uint64_t usedSpaceMiB =
        SD.usedBytes() / (1024ULL * 1024ULL);

    Serial.println("[OK] SD card initialization successful");
    Serial.printf("  Type:           %s\n", getCardTypeName(cardType));

    Serial.printf(
        "  Size: %llu MiB\n",
        static_cast<unsigned long long>(cardSizeMiB)
    );

    Serial.printf(
        "  Filesystem:   %llu MiB\n",
        static_cast<unsigned long long>(totalSpaceMiB)
    );

    Serial.printf(
        "  Used:        %llu MiB\n",
        static_cast<unsigned long long>(usedSpaceMiB)
    );

    return true;
}


// ---------------------------------------------------------
// Root directory
// ---------------------------------------------------------

void listSDRootDirectory() {
    if (!sdCardMounted) {
        Serial.println("[ERROR] SD-Card is not initialized");
        return;
    }

    File root = SD.open("/");

    if (!root) {
        Serial.println(
            "[ERROR] root could not be opened"
        );
        return;
    }

    if (!root.isDirectory()) {
        Serial.println("[ERROR] '/' is not a directory");
        root.close();
        return;
    }

    Serial.println();
    Serial.println("Content of SD Card");

    bool directoryEmpty = true;

    while (true) {
        File entry = root.openNextFile();

        if (!entry) {
            break;
        }

        directoryEmpty = false;

        if (entry.isDirectory()) {
            Serial.printf(
                "  [DIRECTORY] %s\n",
                entry.name()
            );
        } else {
            Serial.printf(
                "  [FILE] %-24s %10llu Byte\n",
                entry.name(),
                static_cast<unsigned long long>(entry.size())
            );
        }

        entry.close();
    }

    if (directoryEmpty) {
        Serial.println("  SD-Card is empty");
    }

    root.close();
}


// ---------------------------------------------------------
// Communication test
// ---------------------------------------------------------

bool testSDCardCommunication() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println(" SD-CARD-COMMUNICATION-TEST");
    Serial.println("==========================================");

    if (!sdCardMounted) {
        Serial.println("SD-Card is not initialized");

        if (!initSDCard()) {
            Serial.println("test quit");
            return false;
        }
    }

    uint16_t passedTests = 0;
    uint16_t failedTests = 0;

    auto report = [&](const char* description, bool successful) {
        printTestResult(description, successful);

        if (successful) {
            passedTests++;
        } else {
            failedTests++;
        }
    };

    // remove testfiles
    SD.remove(SD_TEST_FILE);
    SD.remove(SD_RENAMED_FILE);

    const String originalContent =
        "Smart-Irrigation-System SD-Test\r\n"
        "WRITE_OK\r\n";

    const String appendedContent =
        "APPEND_OK\r\n";

    bool successful =
        writeTextFile(SD_TEST_FILE, originalContent);

    report("write Testfile", successful);

    String receivedContent;

    successful =
        readTextFile(SD_TEST_FILE, receivedContent) &&
        receivedContent == originalContent;

    report("Read file and compare", successful);

    successful =
        appendTextFile(SD_TEST_FILE, appendedContent);

    report("append File", successful);

    receivedContent = "";

    successful =
        readTextFile(SD_TEST_FILE, receivedContent) &&
        receivedContent == originalContent + appendedContent;

    report("check wheather content is received", successful);

    successful =
        SD.rename(SD_TEST_FILE, SD_RENAMED_FILE) &&
        SD.exists(SD_RENAMED_FILE) &&
        !SD.exists(SD_TEST_FILE);

    report("rename testfile", successful);

    successful =
        SD.remove(SD_RENAMED_FILE) &&
        !SD.exists(SD_RENAMED_FILE);

    report("delete testfile", successful);

    // Aufräumen, falls einer der vorherigen Tests fehlgeschlagen ist.
    SD.remove(SD_TEST_FILE);
    SD.remove(SD_RENAMED_FILE);

    Serial.println();
    Serial.println("------------------------------------------");
    Serial.printf("passed:      %u\n", passedTests);
    Serial.printf("failed: %u\n", failedTests);

    if (failedTests == 0) {
        Serial.println(
            "RESULT: SD-Card communication works"
        );
    } else {
        Serial.println(
            "RESULT: ERROR with SD-Card communication"
        );
    }

    Serial.println("------------------------------------------");
    Serial.println("COMMANDS in the Terminal");
    Serial.println("  T = SD-test repeat");
    Serial.println("  L = Show files");
    Serial.println("==========================================");
    Serial.println();

    return failedTests == 0;
}


// ---------------------------------------------------------
// Serielle Testbefehle
// ---------------------------------------------------------

void handleSDCardSerialCommands() {
    while (Serial.available()) {
        const char command = Serial.read();

        switch (command) {
            case 'T':
            case 't':
                testSDCardCommunication();
                break;

            case 'L':
            case 'l':
                listSDRootDirectory();
                break;

            case '\r':
            case '\n':
                break;

            default:
                Serial.println(
                    "Unknown error. T = Test, L = Dateien"
                );
                break;
        }
    }
}