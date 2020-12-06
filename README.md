# **Παράλληλα και Διανεμημένα Συστήματα**  
## **1η Άσκηση**
## Ούρδας Αντώνιος <br /> | ΑΕΜ : 9358 | Email: ourdasav@ece.auth.gr
## Μέτα Λουίς-Κοσμάς <br /> | AEM : 9390 | Email: louismeta@ece.auth.gr  


### **How to build**  

**Προσοχή:** Για να τρέξετε τον κώδικα χρειάζεται να υπάρχει ένας φάκελος με όνομα "matrices" που θα περιέχει τους πέντε πίνακες με μορφή ".mtx" που ζητήθηκαν σε αυτή την εργασία.  

**Αρχείο ```Makefile```:**  
   + Με χρήση της εντολής ```make all``` δημιουργούνται όλα τα ζητούμενα αρχεία.
   + Με την εντολή ``` make test ``` τρέχει την v1 και την v2 με έναν 5x5 πίνακα
   + Με την εντολή ``` make test_v3 ``` τρέχει τους πέντε πίνακες με την v3, v3_cilk με 4 workers, v3_openMP με 4 threads
   + Με την εντολή ``` make test_v4 ``` τρέχει τους πέντε πίνακες με την v4, v4_cilk με 4 workers, v4_openMP με 4 threads
   + H v3_cilk και η v4_cilk χρειάζονται όρισμα για αριθμό workers
   + H v3_openMP και η v4_openMP όρισμα για αριθμό threads
   
**Παραδείγματα εντολών:**
   + ```./v3 ./matrices/mycielskian13.mtx ```  
   + ```./v3_openMp ./matrices/dblp-2010.mtx 4```  
   + ```./v4_cilk ./matrices/com-Youtube.mtx 8```  
   

**Script ```submit_sequential.sh```:**
   + δημιουργεί τα αρχεία v3.txt και v4.txt
   + τρέχει τα v3 και v4 με τους πέντε πίνακες για δύο πυρήνες
      
      
**Script ```submit_parallel.sh```:**  
   + δημιουργεί τα αρχεία v3_cilk.txt, v3_openMP.txt, v4_cilk.txt, v4_openMP.txt
   + τρέχει τα v3_cilk, v3_openMP, v4_cilk, v4_openMP με τους πέντε πίνακες
   + χρησιμοποιεί 2, 4, 5, 10, 15 και 20 πυρήνες 
     
### **Όλες οι παρακάτω μετρήσεις έγιναν στην συστοιχία με την χρήση των παραπάνω scripts**

### Ανάλυση V3 

H υλοποίηση στην V3 μπορεί να αποτυπωθεί στα παρακάτω βήματα:
   + αρχικά παίρνει ως όρισμα έναν πίνακα της μορφής ".mtx"
   + διαβάζει τον πίνακα σε COO κάτω τριγωνική μορφή 
   + με χρήση της coo2csc μετατρέπουμε τον πίνακα σε CSC κάτω τριγωνική μορφή
   + έπειτα ακολουθεί ο αλγόριθμος όπως φαίνεται παρακάτω
   + τέλος εκτυπώνει τον αριθμό των τριγώνων, το διάνυσμα c3 του πίνακα και τον χρόνο που χρειάστηκε για να ολοκληρωθεί ο αλγόριθμος

**Αλγόριθμος**  
Ο αλγόριθμος υλοποιήθηκε με βάση την κάτω τριγωνική CSC μορφή ενός πίνακα. Συγκεκριμένα, διασχίζει το CSC row και CSC col της CSC μορφής και ελέγχει αν ανά δύο τα nodes έχουν κοινούς γείτονες. Όταν βρίσκει κοινούς γείτονες που σημαίνει ότι σχηματίζεται τρίγωνο, τότε αυξάνει το c3 των αντίστοιχων nodes κατά μια μονάδα.  

Ενδεικτικός πίνακας από το script στην συστοιχία για v3 με 2 πυρήνες, v3_cilk με 4 πυρήνες και v3_openMP με 4 πυρήνες:  

|     |  v3 | v3_cilk | v3_openMP |
| --- | --- | --- | --- |
| belgium_osm | 0.022009  | 0.022752 | 0.611180 |
| com-Youtube | 52.206252 | 8.779148 | 7.116321 |
|  dblp-2010  | 0.088070  | 0.298646 | 0.518053 |
|mycielskian13| 21.467567 | 3.188361 | 2.240900 |
|  NACA0015   | 0.170284  | 0.529403 | 0.742436 |


### Ανάλυση V4

H υλοποίηση στην V4 μπορεί να αποτυπωθεί στα παρακάτω βήματα:
   + αρχικά παίρνει ως όρισμα έναν πίνακα της μορφής ".mtx"
   + διαβάζει τον πίνακα σε COO κάτω τριγωνική μορφή 
   + με χρήση της coo2csc μετατρέπουμε τον πίνακα σε CSC κάτω τριγωνική μορφή
   + από την CSC μορφή βρίσκουμε την COO του ανω τριγωνικού
   + προσθέτουμε τις δυο COO μορφές για να πάρουμε ολόκληρο τον πίνακα σε COO μορφή
   + χρησιμοποιούμε πάλι την coo2csc για να πάρουμε την τελική CSC μορφή
   + έπειτα ακολουθεί ο αλγόριθμος που υλοποιεί τον τύπο: ``` c3=(A⊙(AA))e/2 ```
   + τέλος εκτυπώνει τον αριθμό των τριγώνων, το διάνυσμα c3 του πίνακα και τον χρόνο που χρειάστηκε για να ολοκληρωθεί ο αλγόριθμος

**Αλγόριθμος**  
Ο αλγόριθμος υλοποιήθηκε με βάση την μορφή CSC ολόκληρου του πίνακα. Αναλυτικότερα, διασχίζει το CSC row και CSC col της CSC μορφής υπολογίζει κατευθείαν το ``` (A⊙(AA) ```. Στην συνέχεια, πολλαπλασιάζει το αποτέλεσμα με ένα διάνυσμα ``` e ``` με χρήση της συνάρτησης ``` matrixVectorMultiply ```. Τέλος, υπολογίζει το ζητούμενο διάνυσμα c3 διαιρώντας δια δύο το προηγούμενο αποτέλεσμα.

Ενδεικτικός πίνακας από το script στην συστοιχία για v4 με 2 πυρήνες, v4_cilk με 4 πυρήνες και v4_openMP με 4 πυρήνες:  

|     |  v4 | v4_cilk | v4_openMP  |
| --- |:---:|--- | --- |
| belgium_osm | 0.117482  | 0.458160 | 0.834361 |
| com-Youtube | 17.027769 |11.810333 | 16.354739|
|  dblp-2010  | 0.279983  | 0.401437 | 0.420376 |
|mycielskian13| 4.351176  | 2.769288 | 4.038416 |
|  NACA0015   | 1.155859  | 1.578928 | 0.959249 |  


### **Διαγράμματα**  
Τα παρακάτω διαγράμματα προέκυψαν για τους πέντε πίνακες με χρήση των v3_cilk, v3_openMP, v4_cilk και v4_openMP αλγορίθμων για διαφορετικούς αριθμούς πυρήνων. Οι μετρήσεις έγιναν με την χρήση scripts στην συστοιχία και τα διαγράμματα μέσω του Matlab.  

![belgium_osm](https://raw.githubusercontent.com/lkmeta/Parallel-and-Distributed-Systems/main/Matlab%20Figures/belgium_osm.png)  
---
![com-Youtube](https://raw.githubusercontent.com/lkmeta/Parallel-and-Distributed-Systems/main/Matlab%20Figures/com-Youtube.png)  
---
![dblp_2010](https://raw.githubusercontent.com/lkmeta/Parallel-and-Distributed-Systems/main/Matlab%20Figures/dblp_2010.png)  
---
![mycielskian](https://raw.githubusercontent.com/lkmeta/Parallel-and-Distributed-Systems/main/Matlab%20Figures/mycielskian.png) 
---
![NACA0015](https://raw.githubusercontent.com/lkmeta/Parallel-and-Distributed-Systems/main/Matlab%20Figures/NACA0015.png)  

### **Διάγραμμα για σύγκριση χρόνων v3 με v4 με χρήση του Excel**
![v3_v4](https://github.com/lkmeta/Parallel-and-Distributed-Systems/blob/main/Matlab%20Figures/v3_v4_excel.png)
