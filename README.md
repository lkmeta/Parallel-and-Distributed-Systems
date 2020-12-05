# **Παράλληλα και Διανεμημένα Συστήματα**  
## **1η Άσκηση**
## Ούρδας Αντώνιος   ΑΕΜ : 9358
## Μέτα Λουίς-Κοσμάς AEM : 9390


### **HOW TO USE**
   + Με χρήση της εντολής ```make all``` δημιουργούνται όλα τα ζητούμενα αρχεία.
   + Με την εντολή ``` make test_v3``` τρέχει τους πέντε πίνακες με την v3, v3_cilk με 4 workers, v3_openMP με 4 threads
   + Με την εντολή ``` make test_v4``` τρέχει τους πέντε πίνακες με την v4, v4_cilk με 4 workers, v4_openMP με 4 threads
   + H v3_cilk και η v4_cilk έχουν by default 2 workers
   + H v3_openMP και η v4_openMP έχουν by default 2 threads  
   
**Παραδείγματα εντολών:**
   + ```./v3 ./matrices/mycielskian13.mtx ```  
   + ```./v3_openMp ./matrices/dblp-2010.mtx 4```  
   + ```./v4_cilk ./matrices/com-Youtube.mtx 8```  
   
   
ΤΟΔΟΟ ΓΙΑ ΤΟ ΣΚΡΙΠΤΑΚΙ
   

### Ανάλυση V3

H υλοποίηση στην V3 μπορεί να αποτυπωθεί στα παρακάτω βήματα:
   + αρχικά παίρνει ως όρισμα έναν πίνακα της μορφής ".mtx"
   + διαβάζει τον πίνακα σε COO κάτω τριγωνική μορφή 
   + με χρήση της coo2csc μετατρέπουμε τον πίνακα σε CSC κάτω τριγωνική μορφή
   + έπειτα ακολουθεί ο αλγόριθμος όπως φαίνεται παρακάτω
   + τέλος εκτυπώνει τον αριθμό των τριγώνων, το διάνυσμα c3 του πίνακα και τον χρόνο που χρειάστηκε για να ολοκληρωθεί ο αλγόριθμος

**Αλγόριθμος**  
Ο αλγόριθμος υλοποιήθηκε με βάση την κάτω τριγωνική CSC μορφή ενός πίνακα. Συγκεκριμένα, διασχίζει το CSC row και CSC col της CSC μορφής και ελέγχει αν ανά δύο τα nodes έχουν κοινούς γείτονες. Όταν βρίσκει κοινούς γείτονες που σημαίνει ότι σχηματίζεται τρίγωνο, τότε αυξάνει το c3 των αντίστοιχων nodes κατά μια μονάδα.  

Ενδεικτικός πίνακας από το skript στην συστοιχία για v3_cilk με 4 workers και v3_openMP με 4 threads:

| stats.txt |  v3_cilk | v3_openMP  |
| --- |:---:|--- |
| belgium_osm | 0.022752 | 0.611180 |
| com-Youtube | 8.779148 | 7.116321 |
|  dblp-2010  | 0.298646 | 0.518053 |
|mycielskian13| 3.188361 | 2.240900 |
|  NACA0015   | 0.529403 | 0.742436 |


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

Ενδεικτικός πίνακας από το skript στην συστοιχία για v4_cilk με 4 workers και v4_openMP με 4 threads:

| stats.txt |  v4_cilk | v4_openMP  |
| --- |:---:|--- |
| belgium_osm | 0.458160 | 0.834361 |
| com-Youtube | 11.810333 | 16.354739|
|  dblp-2010  | 0.401437 | 0.420376 |
|mycielskian13| 2.769288 | 4.038416 |
|  NACA0015   | 1.578928 | 0.959249 |
