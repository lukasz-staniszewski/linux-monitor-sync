# SOI – Laboratorium – Monitory
#### ŁUKASZ STANISZEWSKI

## 1. Polecenie

Należy napisać program w języku C++ w środowisku Linux realizujący 4 procesy. Występują
dwaj producenci oraz dwaj konsumenci. Tworzone są dwa bufory typu FIFO, które mają być
skończone i do każdego z nich ma być przypisany jeden z producentów, który produkuje
liczby całkowite. Konsumenci natomiast będą na zmianę pobierać z kolejek liczby. Należy
zapewnić synchronizację:
+ nie dopuścić do czytania z pustego bufora,
+ nie dopuścić do zapisu do pełnego bufora,
+ procesy zapisujące i czytające z danego bufora nie mogą sobie przeszkadzać.


## 2. Użyte biblioteki

Poza standardowymi bibliotekami takimi jak **iostream**, **stdio.h** oraz **stdlib.h** użyte
zostaną biblioteki:
+ **pthread.h** -> biblioteka umożliwiająca programowanie wątkowe,
+ **semaphore.h** -> biblioteka implementująca semafory (użyte w gotowej
implementacji monitora w Monitor.h.

## 3. Istotne uwagi implementacyjne

Postanowiłem wykonać zadanie jako jeden program z 4 funkcjami działającymi na
zmianę z użyciem wątków: **producentA()**, **producentB()**, **consumentA()**, **consumentB()** i
korzystającymi z dwóch wskaźników globalnych MonitorQueue* **queueA** oraz **queueB**
reprezentujące oba bufory A i B. Wskaźniki będą wskazywały na pamięć zaalokowaną przed
stworzeniem wątków przez funkcję **allocateQueues()**, a także dealokowaną po zakończeniu
wątków przez funkcję **deallocateQueues()**.

Uwagi co do implementacji kolejki – jest to klasa dziedzicząca z klasy Monitor
wzbogacona o pola Condition **FULL** (aby zapobiec dodawaniu do pełnej kolejki) i **EMPTY**
(aby zapobiec pobieraniu z pustej kolejki), ** *queue** (wskaźnik na tablice integer-ów w
kolejce) oraz **queueSize** – liczba całkowita reprezentująca aktualny rozmiar kolejki.


## 4. Struktura programów

#### Nazwa pliku - funkcjonalność w skrócie:
+ **Monitor.h** - zdefiniowane klasy Semaphore, Condition oraz Monitor;
pobrane ze strony przedmiotu,
+ **MonitorQueue.h** - zdefiniowanie klasy MonitorQueue reprezentująca pojedynczy bufor
współdzielony przez kilka wątków; klasa dziedziczy po klasie Monitor, z dodanymi polami opisanymi w
punkcie 3., a także metodami: addValue(), popValue(), printQueue(); dodatkowo przechowywana jest predefiniowana zmienna
MAX_QUEUE_SIZE,
+ **main.cpp** - zdefiniowanie funkcji producentów i konsumentów, testów, alokacji i
dealokoacji kolejek.

## 5. plik Monitor.h

Jest to gotowy plik pobrany bezpośrednio ze strony przedmiotu. Definiuje on 3 klasy:
Semaphore (implementuje semafor z wykorzystaniem biblioteki semaphore.h), Condition
oraz Monitor (implementacja samego monitora).

## 6. plik MonitorQueue.h

#### Implementacja Bufora jako klasy dziedziczącej z klasy Monitor:

+ Zmienna MAX_QUEUE_SIZE -> maksymalny rozmiar kolejki.

+ Pola klasy:
```
class MonitorQueue: public Monitor{
    Condition FULL, EMPTY;
    int* queue;
    int queueSize;
    …
};
```
+ Metody klasy:
    + Konstruktor:
    ```
    MonitorQueue(){
        this->queue = new int [MAX_QUEUE_SIZE];
        clearQueue();
    }
    ```
    
    + Destruktor:
    ```
    ~MonitorQueue(){
        delete [] this->queue;
    } 
    ```

    + dodajWartosc(wartosc) – dodaje liczbę całkowitą wartosc na koniec kolejki, zwiększa licznik rozmiaru kolejki:
    ```
    void addValue(int value){
        enter();
        if(this->queueSize == MAX_QUEUE_SIZE)
        {
            wait(FULL);
        }
        this->queue[this->queueSize] = value;
        this->queueSize++;
        if(this->queueSize == 1){
            signal(EMPTY);
        }
        leave();
    } 
    ```


    + wyjmijWartosc() – zwraca liczbę całkowitą z pierwszego miejsca kolejki, zmniejsza licznik rozmiaru kolejki:
    ```
    int popValue(){
        int value;
        enter();
        if(this->queueSize == 0){
            wait(EMPTY);
        }
        value = this->queue[0];
        for(int i=0;i<this->queueSize-1; i++){
            this->queue[i] = this->queue[i+1];
        }
        this->queue[queueSize-1] = NULL;
        this->queueSize--;
        if(this->queueSize == MAX_QUEUE_SIZE -1){
            signal(FULL);
        }
        leave();
        return value;
    }
    ```
    + wyswietlKolejke() – wyświetla na konsoli całą kolejkę liczb całkowitych:
    ```
    void printQueue(){
        std::cout<<"|";
        for(int i=0;i<this->queueSize;i++){
            std::cout<<this->queue[i]<<"->";
        }
        std::cout<<"|"<<std::endl;
    } 
    ```
    
## 7. plik main.cpp

Odpowiada za testy, implementacje producentów i konsumentów.

+ Zmienne globalne:

```
MonitorQueue* queueA;
MonitorQueue* queueB;
pthread_t t0, t1, t2, t3;
```

+ producentA(), producentB(), consumentA(), consumentB().

+ allocateQueues(), deallocateQueues():
```
void allocateQueues(){
    queueA = new MonitorQueue();
    queueB = new MonitorQueue();
    std::cout<<"QUEUES ALLOCATED!"<<std::endl;
}
void deallocateQueues(){
    delete queueA;
    delete queueB;
    std::cout<<"QUEUES DEALLOCATED!"<<std::endl;
} 
```

+ Funkcje testujące.


## 8. Testowanie

Testowanie zostanie wykonane za pomocą 4 funkcji w main.cpp:
+ Pierwszy test: test pełny – 2 konsumentów i 2 producentów (jeden dla kolejki A, drugi dla kolejki
B). Każdy wykonuje pobranie / dodanie do kolejki, po czym zasypia na losowy czas (0-2 sekund).
+ Drugi test: test z 2 producentami i 1 konsumentem (jeśli ustalimy ze każdy proces może wykonać
maksymalnie 100 pobrania/dodania do kolejki to po 100 pobraniach wykonanych przez
konsumenta, bufory zostaną w pełni zapełnione, po czym ich wątki zostaną uśpione.
+ Trzeci test: sami konsumenci – program od razu po uruchomieniu powinien zostać uśpiony.
+ Czwarty test: sami producenci – powinni na początku producenci dodać do kolejek maksymalna
możliwą liczbę elementów, po czym zostać uśpione.

Po każdym pobraniu/dodaniu do kolejki wątek będzie wyświetlał:
+ Który to wątek.
+ Co zrobił (pobrał/dodał) i jakiej wartości użył/otrzymał.
+ Na której kolejce została wykonana operacja.
+ Jak wygląda kolejka po tej operacji.

Aby dobrze rozróżniać poszczególne wątki w jednej konsoli, zostaną pokolorowane outputy
poszczególnych wątków. Wynik ostateczny powinien wyglądać następująco:

![image](https://user-images.githubusercontent.com/59453698/110263668-ab9f0400-7fb7-11eb-9082-73aa5f872748.png)

