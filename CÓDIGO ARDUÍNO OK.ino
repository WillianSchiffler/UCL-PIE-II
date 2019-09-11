#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
//#include <EEPROM.h>

SoftwareSerial blueSerial(5, 6); //RX e TX

#define SS_PIN 10
#define RST_PIN 9

// E0 1F 9A 5E
// 03 C7 25 D9
// 51 CD 6A E0

String a = "";
String b = "";
String c = "";
String d = "";
String e = "";

String DADOS[5] = {a, b, c, d, e}; //banco de dados

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup() //Inicia MFRC522
{
  Serial.begin(9600);   // Inicia a serial
  while (!Serial) {}
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT); //Pino do buzzer
  blueSerial.begin(9600);
}

void loop()                                              //MAIN
{
  if (blueSerial.available()) {
    String txtRecebido = "";
    char recebido = blueSerial.read();
    txtRecebido += recebido;

    if (txtRecebido == "A") {                //ADD
      delay(2000);

      ADD(TAG());

    }
    else if (txtRecebido == "E") {                //EXCLUSÃO
      delay(2000);

      EXCLUI(TAG());

    }
    //else if (txtRecebido == "L") {                    //LEITURA
    //  delay(100);

    //  LEITURA();

    //}
    else if (txtRecebido == "X") {                    //LIMPAR DADOS
      delay(100);

      apagarDADOS();

    }
    txtRecebido = "";
  }

  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  conteudo.toUpperCase();

  if (conteudo.substring(1) != "") {
    delay(100);
    OPEN(conteudo.substring(1));
  }
}

void OPEN(String cont)                                                //OPEN
{
  int buz = 0;

  for (int i = 1; i <= 5; i++) {

    if (cont == DADOS[i])
    {
      digitalWrite(2, HIGH); // ativa rele, abre a trava solenoide
      i = i + 5;
      buz = 1;
      delay(2000);           // espera 2 segundos
      digitalWrite(2, LOW);  // desativa rele, fecha a trava solenoide
      blueSerial.print("TAG válida: ");
      blueSerial.write(blueSerial.print(cont));
      blueSerial.println();
    }
    delay(10);
  }

  if (buz == 0) {
    tone(4, 198, 1000);
    blueSerial.print("TAG inválida: ");
    blueSerial.write(blueSerial.print(cont));
    blueSerial.println();
  }
  delay(2000);
}

void ADD(String cont)                                       //ADD
{
  int cheio = 1;
  int cadast = 0;

  for (int j = 1; j <= 5; j++) {
    if (cont == DADOS[j]) {
      cadast = 1;
      j = j + 5;
    }
  }

  if (cadast == 1) {
    blueSerial.write("1"); //TAG já cadastrada!
  }
  if (cadast == 0) {
    for (int i2 = 1; i2 <= 5; i2++) {
      if (DADOS[i2] == "")
      {
        DADOS[i2] = cont;
        delay(10);
        i2 = i2 + 5;
        cheio = 0;
        blueSerial.write("2"); //TAG add com sucesso!
      }
      delay(10);
    }
    if (cheio == 1) {
      blueSerial.write("3"); //Favor excluir uma TAG!
      delay(10);
    }
  }
  delay(2000);
}

void EXCLUI(String cont)                                    //EXCLUI
{
  int nope = 1; //TAG não encontrada

  for (int i3 = 1; i3 <= 5; i3++) {
    if (cont == DADOS[i3])
    {
      DADOS[i3] = "";
      delay(10);
      i3 = i3 + 5;
      nope = 0;
      blueSerial.write("4"); //TAG excluída com sucesso!
    }
    delay(10);
  }
  if (nope == 1) {
    blueSerial.write("5"); //TAG não encontrada!
    delay(10);
  }
  delay(2000);
}



//void LEITURA()                                                 //LEITURA
//{
//  for (int i4 = 1; i4 <= 5; i4++) {
//    blueSerial.write(blueSerial.print(DADOS[i4]));
//    blueSerial.println();
//  }
//  delay(100);
//}

String TAG() {                                                    //TAG

  String str = "";

  for (int n = 1; n <= 5; n++) {
    delay(100);

    // Procura por cartao RFID
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Seleciona o cartao RFID
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }

    String conteudo = "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    conteudo.toUpperCase();

    if (conteudo.substring(1) != "") {
      str = conteudo.substring(1);
      n = n + 5;
    }
  }
  if (str != "") {
    return str;
  }
  else if (str == "") {
    return;
  }
}

void apagarDADOS() {                                                    //APAGAR DADOS

  for (int z = 1; z <= 5; z++) {
    DADOS[z] = "";
  }
  blueSerial.write("6"); //TAG´s apagadas!
  delay(100);
}

