#include <FuzzyRule.h>
#include <FuzzyComposition.h>
#include <Fuzzy.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzyOutput.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzySet.h>
#include <FuzzyRuleAntecedent.h>

//Desired distance (cm).
int distance = 25;

int arrayIndex = 0;
// arrayIndex of the current item in the array
int total = 0;
// stores the cumlative total
int averageDistance = 0;

int TRIG = 10;      // trigger en pin 10
int ECO = 9;      // echo en pin 9
int DURACION;

// variable for storing the distance (cm)
int dist= 0 ;

//PWM pins for the H bridge.
int forward = 5;
int backward = 3;

//Declaring a new fuzzy object.
Fuzzy* fuzzy = new Fuzzy();

void setup(){

//Set sensor Hc04 pines .
pinMode(TRIG, OUTPUT);   // trigger como salida
pinMode(ECO, INPUT);    // echo como entrada
pinMode(forward,OUTPUT);
pinMode(backward,OUTPUT);
//Declare fuzzy input - Distance.
FuzzyInput* distance = new FuzzyInput(1);
//Stablish the rule bases that compose the fuzzy input.
FuzzySet* vf = new FuzzySet(0, 10, 15, 20); //Small distance.
distance->addFuzzySet(vf); //Adding the small distance variable.
FuzzySet* sf = new FuzzySet(20, 25, 25, 30); //Small distance.
distance->addFuzzySet(sf); //Adding the small distance variable.
FuzzySet* centrado = new FuzzySet(30, 40, 40, 45); //Small distance.
distance->addFuzzySet(centrado); //Adding the small distance variable.

FuzzySet* sc = new FuzzySet(45, 100, 110, 120); //Median distance.
distance->addFuzzySet(sc); //Adding the median distance variable.
FuzzySet* vc = new FuzzySet(130, 140, 150, 500); //Big distance.
distance->addFuzzySet(vc); //Adding the big distance variable.

fuzzy->addFuzzyInput(distance); //Adding the distance input to the fuzzy variable.

//Declare fuzzy output - Speed.
FuzzyOutput* velocity = new FuzzyOutput(1);
// Criando os FuzzySet que compoem o FuzzyOutput velocidade
FuzzySet* qbwd = new FuzzySet(-128, -113, -93, -78); // Velocidad lenta
velocity->addFuzzySet(qbwd); // Adicionando o FuzzySet slow en velocity
FuzzySet* sbwd = new FuzzySet(-78, -43, -63, -28); // Velocidad normal
velocity->addFuzzySet(sbwd); // Adicionando o FuzzySet average en velocity
FuzzySet* zero = new FuzzySet(-28, -10, 10, 28); // Velocidad alta
velocity->addFuzzySet(zero); // Adicionando o FuzzySet fast en velocity
FuzzySet* sfwd = new FuzzySet(28, 43, 53, 78); // Velocidade normal
velocity->addFuzzySet(sfwd); // Adicionando o FuzzySet average en velocity
FuzzySet* ffwd = new FuzzySet(78, 93, 113, 128); // Velocidad normal
velocity->addFuzzySet(ffwd); // Adicionando o FuzzySet average en velocity

fuzzy->addFuzzyOutput(velocity); // Adicionando o FuzzyOutput no objeto Fuzzy

//Fuzzy rules.
//IF distance = very close THEN speed = fast bwds
FuzzyRuleAntecedent* ifDistanceSmall = new FuzzyRuleAntecedent();
ifDistanceSmall->joinSingle(vc);
FuzzyRuleConsequent* thenVelocitySlow = new FuzzyRuleConsequent(); // Instancinado um Consequente para a expressao
thenVelocitySlow->addOutput(qbwd);// Adicionando o FuzzySet correspondente ao objeto Consequente
// Instanciando um objeto FuzzyRule
FuzzyRule* fuzzyRule01 = new FuzzyRule(1, ifDistanceSmall,thenVelocitySlow); // Passando o Antecedente e o Consequente da expressao
fuzzy->addFuzzyRule(fuzzyRule01); // Adicionando o FuzzyRule ao objeto Fuzzy

//IF distance = slighty close THEN speed = slow bwds
FuzzyRuleAntecedent* ifDistanceSafe = new FuzzyRuleAntecedent(); // Instanciando um Antecedente para a expresso
ifDistanceSafe->joinSingle(sc); // Adicionando o FuzzySet correspondente ao objeto Antecedente
FuzzyRuleConsequent* thenVelocityAverage = new FuzzyRuleConsequent();// Instancinado um Consequente para a expressao
thenVelocityAverage->addOutput(sbwd);// Adicionando o FuzzySet correspondente ao objeto Consequente
// Instanciando um objeto FuzzyRule
FuzzyRule* fuzzyRule02 = new FuzzyRule(2, ifDistanceSafe,thenVelocityAverage); // Passando o Antecedente e o Consequente da expressao
fuzzy->addFuzzyRule(fuzzyRule02); // Adicionando o FuzzyRule ao objeto Fuzzy

//IF distance = center THEN speed = zero.
FuzzyRuleAntecedent* ifDistanceBig = new FuzzyRuleAntecedent(); // Instanciando um Antecedente para a expresso
ifDistanceBig->joinSingle(centrado); // Adicionando o FuzzySet correspondente ao objeto Antecedente
FuzzyRuleConsequent* thenVelocityFast = new FuzzyRuleConsequent(); // Instancinado um Consequente para a expressao
thenVelocityFast->addOutput(zero);// Adicionando o FuzzySet correspondente ao objeto Consequente
// Instanciando um objeto FuzzyRule
FuzzyRule* fuzzyRule03 = new FuzzyRule(3, ifDistanceBig,
thenVelocityFast); // Passando o Antecedente e o Consequente da expressao
fuzzy->addFuzzyRule(fuzzyRule03); // Adicionando o FuzzyRule ao objeto Fuzzy

//IF distance = slightly far THEN speed = slow fwd
FuzzyRuleAntecedent* ifDistanceSF = new FuzzyRuleAntecedent(); // Instanciando um Antecedente para a expresso
ifDistanceSF->joinSingle(sf); // Adicionando o FuzzySet correspondente ao objeto Antecedente
FuzzyRuleConsequent* thenVelocitySF = new FuzzyRuleConsequent(); // Instancinado um Consequente para a expressao
thenVelocitySF->addOutput(sfwd);// Adicionando o FuzzySet correspondente ao objeto Consequente
// Instanciando um objeto FuzzyRule
FuzzyRule* fuzzyRule04 = new FuzzyRule(4, ifDistanceSF,thenVelocitySF); // Passando o Antecedente e o Consequente da expressao
fuzzy->addFuzzyRule(fuzzyRule04); // Adicionando o FuzzyRule ao objeto Fuzzy

//IF distance = very far THEN speed = fast fwd
FuzzyRuleAntecedent* ifDistanceVF = new FuzzyRuleAntecedent(); // Instanciando um Antecedente para a expresso
ifDistanceVF->joinSingle(vf); // Adicionando o FuzzySet correspondente ao objeto Antecedente
FuzzyRuleConsequent* thenVelocityFF = new FuzzyRuleConsequent(); // Instancinado um Consequente para a expressao
thenVelocityFF->addOutput(ffwd);// Adicionando o FuzzySet correspondente ao objeto Consequente
// Instanciando um objeto FuzzyRule
FuzzyRule* fuzzyRule05 = new FuzzyRule(5, ifDistanceVF, thenVelocityFF); // Passando o Antecedente e o Consequente da expressao
fuzzy->addFuzzyRule(fuzzyRule05); // Adicionando o FuzzyRule ao objeto Fuzzy
}

void loop(){

digitalWrite(TRIG, HIGH);     // generacion del pulso a enviar
delay(1);       // al pin conectado al trigger
digitalWrite(TRIG, LOW);    // del sensor
DURACION = pulseIn(ECO, HIGH);  // con funcion pulseIn se espera un pulso alto en ECO
dist = DURACION / 58.2;    // distancia medida en centimetros

fuzzy->setInput(1, dist);

fuzzy->fuzzify();

int output = fuzzy->defuzzify(1);

//Send the signal to the Arduino to activate the necessary PWM output.
if(output <0)
{
analogWrite(forward,200+output);
}
else if(output == 0)
{
analogWrite(forward,0);
analogWrite(backward,0);
}
else if(output > 0)
{
analogWrite(backward,100+output);
}

// wait 100 milli seconds before looping again
delay(100);
}
