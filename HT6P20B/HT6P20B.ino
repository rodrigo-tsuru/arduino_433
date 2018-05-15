/*
  ARDUINO DECODIFICADOR HT6P20B COM RESISTOR DE 2M2 NO OSC.
  ESTE CÓDIGO NÃO USA TIMER, INTERRUPÇÃO EXTERNA E NEM PINO DEFINIDO DE ENTRADA.
  ELE MEDE O TEMPO DO PILOT PERIOD E COMPARA SE ESTÁ DENTRO DA FAIXA DEFINIDA,
  SE TIVER ELE PASSA PARA CODE PERIOD E FAZ AS MEDIDÇÕES DE TEMPO EM NIVEL ALTO
  SE TIVER DENTRO DA FAIXA ACRECENTA BIT 1 OU 0 NA VARIAVEL _DATA CASO NÃO ZERA AS VARIÁVEIS E RE-INICIA
  APOS RECEBER TODOS OS BITS ELE PEGA OS 4 BITS DO ANTE CODE E TESTA PARA SABER SE O CÓDIGO FOI RECEBIDO
  CORRETAMENTE, CASO RECEBIDO CERTO ELE COLOCA A VARIAVEL ANTCODE EM 1.

  CRIADO POR: RODRIGO KEY TSURU BASEADO EM FONTE ORIGINAL DE JACQUES DANIEL MORESCO (ibisul@ibisul.com.br)
  DATA: 14/05/2018 USANDO ARDUINO 0022.
  FONES:11 959202020
  E-MAIL: caixapostal@gmail.com
  Permitido o uso público, mas deve-se manter o nome do autor.
*/

int x, startbit, ctr, dataok, t1, larpulso, larpulso1, larpulso2, larpulso3, larpulso4, bt1, bt2, antcode = 0;
unsigned long _data = 0;
unsigned long _antcode = 0;
unsigned long _addr = 0;
unsigned long _state = 0;
unsigned long _dur, _dur1;

int counter = 1;////////////////////CANAL 1//////////////"variavel Counter"
int countero = 1;////////////////////CANAL 2//////////////"variavel Countero"

const byte pinRF_RX = 2;      //Pin where RF Receiver Module is connected. If necessary, change this for your project
const byte pinRF_TX = 4;      //Pin where RF Transmitter Module is connected. If necessary, change this for your project
const byte pinButton_TX = 3;   //Pin where "Enable transmission button" is connected. Change this field as pin used in your project
const byte pinLearnButton = 5; //Pin where "Learn button" is connected. Change this field as pin used in your project
const byte pinTesteLED = 6;    //Pin where "Learning Led" is connected
const byte pinReceiveLED = 13; //On board Led

////////PODE ADICIONAR VARIOS CANAIS...
////////CADA CANAL DEVE TER DUAS VARIAVEIS

void setup()

{
  Serial.begin(115200);
  pinMode(pinRF_RX,  INPUT);
  pinMode(pinReceiveLED, OUTPUT);
  pinMode(pinTesteLED, OUTPUT);
}

void loop()

{

  digitalWrite(pinReceiveLED, digitalRead(pinRF_RX)); //Pisca o LED conforme o valor da entrada digital, testar interferencias.
  if (startbit == 0)
  { // Testa o tempo piloto até o Bit de inicio;
    _dur = pulseIn(pinRF_RX, LOW);
    if (_dur > 8000 && _dur < 12000 && startbit == 0)
    {
      larpulso = _dur / 23;
      larpulso1 = larpulso - 50;
      larpulso2 = larpulso + 50;
      larpulso3 = larpulso + larpulso - 50;
      larpulso4 = larpulso + larpulso + 50;
      startbit = 1;
      _dur = 0;
      _data = 0;
      dataok = 0;
      ctr = 0;
    }
  }

  // Se o Bit de inicio OK ENTÃO Inicia a medição do tempo em Nivel ALTO dos sinais, e testa se o tempo está na faixa.
  if (startbit == 1 && dataok == 0 && ctr < 28)
  {
    ++ctr;
    _dur1 = pulseIn(pinRF_RX, HIGH);
    if (_dur1 > larpulso1 && _dur1 < larpulso2)    // Se a largura de pulso é entre 1/4000 e 1/3000 segundos
    {
      _data = (_data << 1) + 1;      // anexar um * 1 * para a extremidade mais à direita do buffer
    }
    else if (_dur1 > larpulso3 && _dur1 < larpulso4)  // Se a largura de pulso é entre 2/4000 e 2/3000 segundos
    {
      _data = (_data << 1);       // anexar um * 0 * para a extremidade mais à direita do buffer
    }
    else
    {
      /* força finalização do laço */
      startbit = 0;
    }

  }

  if (ctr == 28)
  {
    Serial.println("Retornou 28 bits");

    if ((_data & 0xF) == 0x5) { // Se os quatro ultimos bits forem 0101, entao eh uma mensagem valida
      antcode = 1;
      _addr = _data >> 6;
      _state = bitRead(_data, 4) * 2 + bitRead(_data, 5);
      Serial.println(_addr, HEX);
      Serial.println(_state, HEX);
    }

    if (antcode == 1)
    { // Se foram recebidos todos os 28 Bits, o valor vai para a variavel _data e pode ser usada como exemplo abaixo.
      dataok = 1;
      Serial.print("Address:");
      Serial.println(_addr, HEX);
      Serial.print("State:");
      Serial.println(_state,HEX);
      
      Serial.println(counter, DEC);

      ctr = 0;
      startbit = 0;
      antcode = 0;
      delay(100);
      ////////////////////CANAL 1//////////////"variavel Counter"
      /////Colocar codigo botao liga
      if (_data == 147032053)
      { if (counter == 1)
          digitalWrite(pinTesteLED, HIGH);
        counter++;
        if (counter == 3)
          digitalWrite(pinTesteLED, HIGH);
        if (counter == 3)
          counter = (1);

      } else {
        ////////////////////CANAL 2//////////////"variavel Countero"
        /////Colocar Codigo botao Desliga
        if (_data == 137490517)

        {
          if (countero == 1)
            digitalWrite(pinTesteLED, LOW);
          countero++;
          if (countero == 3)
            digitalWrite(pinTesteLED, LOW);
          if (countero == 3)
            countero = (1);

        }
      }
    }
  }
}
