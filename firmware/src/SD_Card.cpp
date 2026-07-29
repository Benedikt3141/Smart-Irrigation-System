#include "functions.h"

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "pindefinitions.h"

namespace {

// Für den ersten Hardwaretest bewusst nur 4 MHz.
// Falls die Karte nicht erkannt wird, testweise 1000000UL verwenden.
constexpr uint32_t SD_SPI_FREQUENCY = 4000000UL;

constexpr char SD_TEST_FILE[] = "/SIS_TST.TXT";
constexpr char SD_RENAMED_FILE[] = "/SIS_TST2.TXT";

bool sdCardMounted = false;


// ---------------------------------------------------------
// Hilfsfunktionen
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
// SD-Karte initialisieren
// ---------------------------------------------------------

bool initSDCard() {
    Serial.println();
    Serial.println("Initialisiere SD-Karte ...");

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
        Serial.println("[FEHLER] SD-Karte konnte nicht gemountet werden.");
        return false;
    }

    const uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("[FEHLER] Keine SD-Karte erkannt.");
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

    Serial.println("[OK] SD-Karte erfolgreich initialisiert.");
    Serial.printf("  Typ:           %s\n", getCardTypeName(cardType));

    Serial.printf(
        "  Kartengroesse: %llu MiB\n",
        static_cast<unsigned long long>(cardSizeMiB)
    );

    Serial.printf(
        "  Dateisystem:   %llu MiB\n",
        static_cast<unsigned long long>(totalSpaceMiB)
    );

    Serial.printf(
        "  Belegt:        %llu MiB\n",
        static_cast<unsigned long long>(usedSpaceMiB)
    );

    return true;
}


// ---------------------------------------------------------
// Hauptverzeichnis anzeigen
// ---------------------------------------------------------

void listSDRootDirectory() {
    if (!sdCardMounted) {
        Serial.println("[FEHLER] SD-Karte ist nicht initialisiert.");
        return;
    }

    File root = SD.open("/");

    if (!root) {
        Serial.println(
            "[FEHLER] Hauptverzeichnis konnte nicht geoeffnet werden."
        );
        return;
    }

    if (!root.isDirectory()) {
        Serial.println("[FEHLER] '/' ist kein Verzeichnis.");
        root.close();
        return;
    }

    Serial.println();
    Serial.println("Inhalt der SD-Karte:");

    bool directoryEmpty = true;

    while (true) {
        File entry = root.openNextFile();

        if (!entry) {
            break;
        }

        directoryEmpty = false;

        if (entry.isDirectory()) {
            Serial.printf(
                "  [VERZEICHNIS] %s\n",
                entry.name()
            );
        } else {
            Serial.printf(
                "  [DATEI] %-24s %10llu Byte\n",
                entry.name(),
                static_cast<unsigned long long>(entry.size())
            );
        }

        entry.close();
    }

    if (directoryEmpty) {
        Serial.println("  SD-Karte ist leer.");
    }

    root.close();
}


// ---------------------------------------------------------
// Kommunikationstest
// ---------------------------------------------------------

bool testSDCardCommunication() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println(" SD-KARTEN-KOMMUNIKATIONSTEST");
    Serial.println("==========================================");

    if (!sdCardMounted) {
        Serial.println("SD-Karte ist nicht initialisiert.");

        if (!initSDCard()) {
            Serial.println("Test abgebrochen.");
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

    // Alte Testdateien entfernen.
    // Normale Dateien auf der SD-Karte werden nicht verändert.
    SD.remove(SD_TEST_FILE);
    SD.remove(SD_RENAMED_FILE);

    const String originalContent =
        "Smart-Irrigation-System SD-Test\r\n"
        "WRITE_OK\r\n";

    const String appendedContent =
        "APPEND_OK\r\n";

    bool successful =
        writeTextFile(SD_TEST_FILE, originalContent);

    report("Testdatei schreiben", successful);

    String receivedContent;

    successful =
        readTextFile(SD_TEST_FILE, receivedContent) &&
        receivedContent == originalContent;

    report("Datei lesen und vergleichen", successful);

    successful =
        appendTextFile(SD_TEST_FILE, appendedContent);

    report("Daten an Datei anhaengen", successful);

    receivedContent = "";

    successful =
        readTextFile(SD_TEST_FILE, receivedContent) &&
        receivedContent == originalContent + appendedContent;

    report("Angehaengte Daten pruefen", successful);

    successful =
        SD.rename(SD_TEST_FILE, SD_RENAMED_FILE) &&
        SD.exists(SD_RENAMED_FILE) &&
        !SD.exists(SD_TEST_FILE);

    report("Testdatei umbenennen", successful);

    successful =
        SD.remove(SD_RENAMED_FILE) &&
        !SD.exists(SD_RENAMED_FILE);

    report("Testdatei loeschen", successful);

    // Aufräumen, falls einer der vorherigen Tests fehlgeschlagen ist.
    SD.remove(SD_TEST_FILE);
    SD.remove(SD_RENAMED_FILE);

    Serial.println();
    Serial.println("------------------------------------------");
    Serial.printf("Bestanden:      %u\n", passedTests);
    Serial.printf("Fehlgeschlagen: %u\n", failedTests);

    if (failedTests == 0) {
        Serial.println(
            "ERGEBNIS: SD-KARTENKOMMUNIKATION FUNKTIONIERT"
        );
    } else {
        Serial.println(
            "ERGEBNIS: FEHLER BEI DER SD-KARTENKOMMUNIKATION"
        );
    }

    Serial.println("------------------------------------------");
    Serial.println("Befehle im seriellen Monitor:");
    Serial.println("  T = SD-Test wiederholen");
    Serial.println("  L = Dateien anzeigen");
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
                    "Unbekannter Befehl. T = Test, L = Dateien"
                );
                break;
        }
    }
}