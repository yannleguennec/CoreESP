#include "PluginButton.h"

#define debouncedBit 0x001
#define changedBit   0x010
#define unstableBit  0x100

#define getBit(v,b) (((v)&(b))!=0)
#define setBit(v, b) ((v)|=(b))
#define unsetBit(v, b) ((v)&=(~(b)))
#define toggleBit(v,b) ((v)^=(b))

#define getDebounced()    getBit( buttonState,    debouncedBit )
#define unsetDebounced()  unsetBit( buttonState,  debouncedBit )
#define toggleDebounced() toggleBit( buttonState, debouncedBit )

#define getChanged()      getBit(buttonState,     changedBit )
#define setChanged()      setBit(buttonState,     changedBit )
#define unsetChanged()    unsetBit(buttonState,   changedBit )

#define getUnstable()     getBit( buttonState,    unstableBit )
#define setUnstable()     setBit( buttonState,    unstableBit )
#define toggleUnstable()  toggleBit( buttonState, unstableBit )

#define buttonPressed()   (getChanged() && getDebounced())
#define buttonReleased()  (getChanged() && !getDebounced())

#define readButton() (digitalRead(buttonPin) ^ buttonInvert)

void PluginButton::setup(void)
{
  PANIC_DEBUG();

  buttonPin = 0;
  buttonInvert = true; // normally a button works with invert logic and pull-ups

  debounceMethod = debounceMethodShiftRegister;
  debounceTimeout = 50;

  if (readButton())
    setBit(buttonState, debouncedBit | unstableBit);
}

void PluginButton::setMethod(int method)
{
  PANIC_DEBUG();

  debounceMethod = method;
}

void PluginButton::setPin(int pin)
{
  PANIC_DEBUG();

  buttonPin = pin;
}

void PluginButton::setTimeout(unsigned long timeout)
{
  PANIC_DEBUG();

  debounceTimeout = timeout;
}

bool PluginButton::debounce(void)
{
  bool buttonState = readButton();

  // D'abord, on considère qu'il n'y a pas eu de changement
  unsetChanged();

  switch ( debounceMethod )
  {
    case debounceMethodNone:
      {
        // Pas de debounce
        // ---------------
        // Principe :      Lorsqu'un changement est détecté, il est pris en compte tel quel.
        // Avantages :     Code simple.
        // Inconvévients : en cas de parasite, l'etat vacille.

        if ( buttonState != getDebounced() )
        {
          // L'etat a changé, stocke le changement.
          toggleDebounced();
          setChanged();
        }
        break;
      }
    case debounceMethodLockOut:
      {
        // Debounce avec la méthode du lock-out
        // ------------------------------------
        // Principe :      Lorsqu'un changement est détecté, il est pris en compte
        //                 et on ne regarde plus avant que le timeout soit expiré.
        // Avantages :     Le changement est pris en compte immédiatement, s'il n'y a pas eu de changement recemment.
        // Inconvenients : Si le changement est dû à un parasite, il est pris en compte quand même.

        static unsigned long lastDebounceTime = 0;
        if ((millis() - lastDebounceTime >= debounceTimeout) &&
            ( buttonState != getDebounced() ))
        {
          // Le timeout est passé et l'état a changé,
          toggleDebounced();
          setChanged();

          // Reset du timeout
          lastDebounceTime = millis();
        }

        break;
      }
    case debounceMethodTimer:
      {
        // Debounce avec la méthode du timer
        // ---------------------------------
        // Principe :      Prend en compte l'état s'il n'a pas changé depuis timeout.
        //                 Tant que la valeur vacille (instable), on reset le timeout.
        // Avantages :     Quand le changement est pris en compte, on est sûr qu'il est ok.
        //                 Le timeout est plus simple à appréhender qu'avec la méthode shift register
        // Inconvénients : Le changement est pris en compte au bout du timeout, pas immédiatement

        static unsigned long lastDebounceTime = millis();

        if ( (buttonState !=  getDebounced()) &&
             (( millis() - lastDebounceTime ) >= debounceTimeout) )
        {
          // Le timeout est passé et l'état a changé,
          toggleDebounced();
          setChanged();

          // Reset du timeout
          lastDebounceTime = millis();
        }

        // Si l'état vacille
        if (buttonState != getUnstable())
        {
          // L'état est instable, conserve cet état
          toggleUnstable();

          // Reset du timeout
          lastDebounceTime = millis();
        }
        break;
      }
    case debounceMethodShiftRegister:
      {
        // Debounce avec la méthode du registre à décalage
        // -----------------------------------------------
        // Principe :      Stock un historique de plusieurs valeurs dans un registre à décalage et
        //                 vérifie qu'elles sont toutes à 0 ou à 1 pour prendre en compte l'état.
        //                 Quand l'état vacille, on atteint pas les valeurs de prise en compte (que des 0 ou que des 1).
        // Avantages :     Quand le changement est pris en compte, on est sûr qu'il est ok.
        //                 Code plus rapide et concis que la méthode du timer.
        //                 Utilise à fond les capacités binaire du micro-controlleur.
        // Inconvénients : Le changement est pris en compte au bout de 16 cycles (ou 8 tours si on utilise un byte), pas immédiatement
        //                 Le timeout réel doit être calculer en fct du nombre de bits dans le registre
        //                 Le nombre de valeurs dans l'historique est bridé à la taille du type utilisé pour le registre
        //                 Avec un delay de 5ms et 8 positions on obtient 40ms environ.

        // On peut choisir le type pour avoir différentes tailles de registre
        typedef byte shiftRegister;  // 8 positions
        // typedef word shiftRegister;  // 16 positions
        // typedef unsigned long dword;
        // typedef dword shiftRegister; // 32 positions

        static shiftRegister debounceRegister = 0;
#define all0 ((shiftRegister)0)
#define all1 ((shiftRegister)-1)

        // Décale de 1 vers la gauche toutes les positions et place l'état en position 0
        debounceRegister <<= 1;
        debounceRegister |= buttonState;

        if ( ((debounceRegister == all0) && ( !getDebounced() ) ) ||
             ((debounceRegister == all1) && (  getDebounced() ) ) )
        {
          // L'état est stable, il a changé.
          toggleDebounced();
          setChanged();
        }
        break;
      }
    default:
      {
        // Erreur, unknown method !!!

        Serial.println(__PRETTY_FUNCTION__);
        Serial.println("Bad debounce method.");
      }
  }
  return getChanged();
}

void PluginButton::update(void)
{
  if (debounce())
  {
    String msg = _deviceName;
    msg += '=';
    msg += getDebounced();

    Serial.print("Debounce: ");
    Serial.println(msg.begin());
  }
}

void PluginButton::loop(void)
{
  schedule( update, 5 );
}

void PluginButton::webSubmit( void )
{
  PANIC_DEBUG();

  __super::webSubmit();

  buttonPin = atoi( WebServer.arg("pin").begin() );
  buttonInvert = atoi( WebServer.arg("invert").begin() );
}

CorePlugins* PluginButton::factory(void)
{
  PANIC_DEBUG();

  return new PluginButton(*this);
}


PluginButton pluginButton();
