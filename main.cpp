#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

// ==========================================
// STRUKTUR DATA ENTITAS KENDARAAN
// ==========================================
struct Vehicle {
    string plateNumber;
    int entryHour;
    int exitHour;
    int parkingFee;
};

// ==========================================
// CUSTOM DYNAMIC ARRAY (PENGGANTI std::vector)
// ==========================================
class VehicleArray {
private:
    Vehicle* arr;
    int capacity;
    int size;

    void resize() {
        capacity *= 2;
        Vehicle* newArr = new Vehicle[capacity];
        for (int i = 0; i < size; i++) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = newArr;
    }

public:
    VehicleArray() {
        capacity = 4;
        size = 0;
        arr = new Vehicle[capacity];
    }

    ~VehicleArray() {
        delete[] arr;
    }

    // Salin konstruktor untuk penanganan memori yang aman saat passing object
    VehicleArray(const VehicleArray& other) {
        capacity = other.capacity;
        size = other.size;
        arr = new Vehicle[capacity];
        for (int i = 0; i < size; i++) {
            arr[i] = other.arr[i];
        }
    }

    VehicleArray& operator=(const VehicleArray& other) {
        if (this != &other) {
            delete[] arr;
            capacity = other.capacity;
            size = other.size;
            arr = new Vehicle[capacity];
            for (int i = 0; i < size; i++) {
                arr[i] = other.arr[i];
            }
        }
        return *this;
    }

    void push_back(Vehicle v) {
        if (size == capacity) resize();
        arr[size++] = v;
    }

    void erase(int index) {
        if (index < 0 || index >= size) return;
        for (int i = index; i < size - 1; i++) {
            arr[i] = arr[i + 1];
        }
        size--;
    }

    int getSize() const { return size; }

    Vehicle& operator[](int index) { return arr[index]; }
    const Vehicle& operator[](int index) const { return arr[index]; }
};

// ==========================================
// 1. MODUL ANTRIAN (CUSTOM QUEUE - LINKED LIST)
// ==========================================
struct QNode {
    Vehicle data;
    QNode* next;
    QNode(Vehicle v) : data(v), next(nullptr) {}
};

class ParkingQueue {
private:
    QNode* head;
    QNode* tail;
    int count;

public:
    ParkingQueue() : head(nullptr), tail(nullptr), count(0) {}
    
    ~ParkingQueue() {
        Vehicle dummy;
        while (dequeue(dummy));
    }
    
    void enqueue(string plate) {
        // Inisialisasi awal jam masuk/keluar/biaya = 0 saat di antrian gerbang
        QNode* newNode = new QNode({plate, 0, 0, 0});
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
        cout << "[QUEUE] Kendaraan " << plate << " masuk antrian gerbang.\n";
    }

    bool dequeue(Vehicle& v) {
        if (head == nullptr) return false;
        QNode* temp = head;
        v = temp->data;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete temp;
        count--;
        return true;
    }

    void display() {
        if (!head) { cout << "Antrian gerbang kosong.\n"; return; }
        QNode* temp = head;
        cout << "Antrian Masuk: ";
        while (temp) {
            cout << "[" << temp->data.plateNumber << "] -> ";
            temp = temp->next;
        }
        cout << "NULL\n";
    }

    int getCount() const { return count; }
};

// ==========================================
// 2. MODUL RIWAYAT (CUSTOM STACK - LINKED LIST)
// ==========================================
struct SNode {
    Vehicle data;
    SNode* next;
    SNode(Vehicle v) : data(v), next(nullptr) {}
};

class HistoryStack {
private:
    SNode* top;
    int count;

public:
    HistoryStack() : top(nullptr), count(0) {}

    ~HistoryStack() {
        Vehicle dummy;
        while (pop(dummy));
    }

    void push(Vehicle v) {
        SNode* newNode = new SNode(v);
        newNode->next = top;
        top = newNode;
        count++;
    }

    bool pop(Vehicle& v) {
        if (top == nullptr) return false;
        SNode* temp = top;
        v = temp->data;
        top = top->next;
        delete temp;
        count--;
        return true;
    }

    void displayTop() {
        if (top) {
            cout << "Kendaraan terakhir keluar: " << top->data.plateNumber 
                 << " (Biaya: Rp" << top->data.parkingFee << ")\n";
        } else {
            cout << "Belum ada riwayat transaksi keluar.\n";
        }
    }
    
    int getCount() const { return count; }
    
    // Guna kebutuhan kalkulasi total pendapatan finansial di thread laporan
    int getTotalRevenue() {
        int total = 0;
        SNode* temp = top;
        while (temp) {
            total += temp->data.parkingFee;
            temp = temp->next;
        }
        return total;
    }
};

// ==========================================
// 3. MODUL PENCARIAN (DIVIDE & CONQUER)
// ==========================================
class ParkingDatabase {
public:
    VehicleArray parkedVehicles;

    // Merge Sort - Menggabungkan dua sub-array terurut
    void merge(VehicleArray& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        // Alokasi memori dynamic array lokal murni native
        Vehicle* L = new Vehicle[n1];
        Vehicle* R = new Vehicle[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].plateNumber <= R[j].plateNumber) {
                arr[k] = L[i]; i++;
            } else {
                arr[k] = R[j]; j++;
            }
            k++;
        }
        while (i < n1) { arr[k] = L[i]; i++; k++; }
        while (j < n2) { arr[k] = R[j]; j++; k++; }

        delete[] L;
        delete[] R;
    }

    // Merge Sort - Rekursif Memecah Array
    void mergeSort(VehicleArray& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    // Binary Search
    int binarySearch(const VehicleArray& arr, int left, int right, const string& target) {
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid].plateNumber == target) return mid;
            if (arr[mid].plateNumber < target) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
};

// ==========================================
// 4. APLIKASI UTAMA & INTEGRASI MULTITHREADING
// ==========================================
class SmartParkingApp {
private:
    ParkingQueue queue;
    HistoryStack history;
    ParkingDatabase db;
    mutex mtx;
    const int TARIF_PER_JAM = 3000;

    // Fungsi utilitas menghitung biaya parkir berdasarkan input jam
    int hitungBiaya(int masuk, int keluar) {
        int durasi = keluar - masuk;
        // Penanganan jika melewati tengah malam (Contoh: Masuk jam 23, Keluar jam 02)
        if (durasi < 0) {
            durasi += 24;
        }
        if (durasi == 0) durasi = 1; // Minimum charge 1 jam
        return durasi * TARIF_PER_JAM;
    }

public:
    void menuTambahAntrian() {
        string plat;
        cout << "Masukkan Plat Nomor Kendaraan: ";
        cin >> plat;
        queue.enqueue(plat);
    }

    void menuParkirkan() {
        Vehicle v;
        if (queue.dequeue(v)) {
            int jamMasuk;
            cout << "Kendaraan [" << v.plateNumber << "] dipanggil ke slot parkir.\n";
            cout << "Input Jam Masuk (Format 0-23): ";
            cin >> jamMasuk;
            
            if (jamMasuk < 0 || jamMasuk > 23) {
                cout << "[ERROR] Format jam tidak valid!\n";
                queue.enqueue(v.plateNumber); // Kembalikan ke antrian gerbang
                return;
            }
            
            v.entryHour = jamMasuk;
            db.parkedVehicles.push_back(v);
            cout << "[PARKIR] " << v.plateNumber << " resmi menempati slot parkir.\n";
        } else {
            cout << "[ERROR] Tidak ada kendaraan di antrian gerbang!\n";
        }
    }

    void menuKeluarkan() {
        string plat;
        cout << "Masukkan Plat Kendaraan yang Akan Keluar: ";
        cin >> plat;

        for (int i = 0; i < db.parkedVehicles.getSize(); i++) {
            if (db.parkedVehicles[i].plateNumber == plat) {
                int jamKeluar;
                cout << "Kendaraan ditemukan. Jam masuk tercatat: " << db.parkedVehicles[i].entryHour << "\n";
                cout << "Input Jam Keluar (Format 0-23): ";
                cin >> jamKeluar;

                if (jamKeluar < 0 || jamKeluar > 23) {
                    cout << "[ERROR] Format jam tidak valid!\n";
                    return;
                }

                db.parkedVehicles[i].exitHour = jamKeluar;
                db.parkedVehicles[i].parkingFee = hitungBiaya(db.parkedVehicles[i].entryHour, jamKeluar);

                cout << "[PROSES TRANSALKSI] Durasi Terhitung. Biaya: Rp" << db.parkedVehicles[i].parkingFee << "\n";
                
                // Simpan data lengkap ke stack riwayat
                history.push(db.parkedVehicles[i]); 
                db.parkedVehicles.erase(i);
                cout << "[KELUAR] Kendaraan " << plat << " telah meninggalkan area parkir.\n";
                return;
            }
        }
        cout << "[ERROR] Kendaraan tidak ditemukan aktif di area parkir.\n";
    }

    void menuUndo() {
        Vehicle v;
        if (history.pop(v)) {
            v.exitHour = 0;
            v.parkingFee = 0; // Reset status transaksi karena dibatalkan
            db.parkedVehicles.push_back(v);
            cout << "[UNDO SUCCESS] Transaksi dibatalkan. " << v.plateNumber << " kembali ke area parkir.\n";
        } else {
            cout << "[ERROR] Riwayat kosong, tidak ada transaksi yang bisa dibatalkan.\n";
        }
    }

    void menuCari() {
        string plat;
        cout << "Masukkan Plat Nomor yang dicari: ";
        cin >> plat;

        if (db.parkedVehicles.getSize() == 0) {
            cout << "Area parkir sedang kosong.\n"; return;
        }

        // Divide & Conquer: Salin data ke array temporer lalu di-Sort dan di-Search
        VehicleArray tempArr = db.parkedVehicles;
        db.mergeSort(tempArr, 0, tempArr.getSize() - 1);
        int idx = db.binarySearch(tempArr, 0, tempArr.getSize() - 1, plat);

        if (idx != -1) {
            cout << "[FOUND] Kendaraan " << plat << " aktif parkir sejak jam " << tempArr[idx].entryHour << ".\n";
        } else {
            cout << "[NOT FOUND] Kendaraan " << plat << " tidak ditemukan di dalam area parkir.\n";
        }
    }

    // --- WORKER THREAD UNTUK PROSES PARALEL LAPORAN ---
    void taskHitungAntrian(int& res) {
        this_thread::sleep_for(chrono::milliseconds(200)); 
        lock_guard<mutex> lock(mtx);
        res = queue.getCount();
    }

    void taskHitungParkir(int& res) {
        this_thread::sleep_for(chrono::milliseconds(250)); 
        lock_guard<mutex> lock(mtx);
        res = db.parkedVehicles.getSize();
    }

    void taskHitungKeluar(int& res) {
        this_thread::sleep_for(chrono::milliseconds(150));
        lock_guard<mutex> lock(mtx);
        res = history.getCount();
    }

    // Thread baru untuk kalkulasi total pendapatan finansial secara independen
    void taskHitungPendapatan(int& res) {
        this_thread::sleep_for(chrono::milliseconds(300));
        lock_guard<mutex> lock(mtx);
        res = history.getTotalRevenue();
    }

    void menuLaporan() {
        cout << "\n=== MEMPROSES DASHBOARD STATISTIK (MULTITHREADING) ===\n";
        int tQueue = 0, tParkir = 0, tKeluar = 0, tRevenue = 0;

        auto start = chrono::high_resolution_clock::now();

        // Eksekusi paralel 4 fungsi kalkulasi data mendasar pada core CPU terpisah
        thread th1(&SmartParkingApp::taskHitungAntrian, this, ref(tQueue));
        thread th2(&SmartParkingApp::taskHitungParkir, this, ref(tParkir));
        thread th3(&SmartParkingApp::taskHitungKeluar, this, ref(tKeluar));
        thread th4(&SmartParkingApp::taskHitungPendapatan, this, ref(tRevenue));

        th1.join();
        th2.join();
        th3.join();
        th4.join();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;

        cout << "\n====================================\n";
        cout << "  LAPORAN REAL-TIME SISTEM PARKIR   \n";
        cout << "====================================\n";
        cout << " Jumlah Kendaraan Mengantri  : " << tQueue << " unit\n";
        cout << " Jumlah Kendaraan Aktif Parkir: " << tParkir << " unit\n";
        cout << " Jumlah Kendaraan Selesai     : " << tKeluar << " unit\n";
        cout << " Total Pendapatan Finansial   : Rp" << tRevenue << "\n";
        cout << "------------------------------------\n";
        cout << " Over-head Latency Thread     : " << duration.count() << " ms\n";
        cout << "====================================\n";
    }

    void run() {
        int pilihan = -1;
        while (pilihan != 0) {
            cout << "\n=== SMART PARKING MANAGEMENT ===\n";
            cout << "1. Tambah Antrian Masuk (Queue)\n";
            cout << "2. Panggil Antrian ke Area Parkir\n";
            cout << "3. Proses Kendaraan Keluar & Hitung Biaya\n";
            cout << "4. Undo Transaksi Keluar Terakhir (Stack)\n";
            cout << "5. Pencarian Kendaraan Aktif (Divide & Conquer)\n";
            cout << "6. Tampilkan Laporan Omset (Multithreading)\n";
            cout << "7. Monitoring Log Antrian & Histori Teratas\n";
            cout << "0. Keluar Aplikasi\n";
            cout << "Pilih menu: ";
            cin >> pilihan;

            // Deteksi jika input bukan angka (Input Stream Failure)
            if (cin.fail()) {
                cin.clear(); // 1. Kembalikan cin ke status normal
                cin.ignore(10000, '\n'); // 2. Buang sisa karakter yang salah di buffer
                cout << "[ERROR] Input tidak valid! Harap masukkan angka.\n";
                pilihan = -1; // Reset pilihan agar loop tetap berjalan
                continue;     // Kembali ke awal loop
            }
            
            switch (pilihan) {
                case 1: menuTambahAntrian(); break;
                case 2: menuParkirkan(); break;
                case 3: menuKeluarkan(); break;
                case 4: menuUndo(); break;
                case 5: menuCari(); break;
                case 6: menuLaporan(); break;
                case 7: 
                    queue.display(); 
                    history.displayTop(); 
                    break;
                case 0: cout << "Sistem dimatikan. Terima kasih.\n"; break;
                default: cout << "[ERROR] Menu tidak tersedia!\n";
            }
        }
    }
};

int main() {
    SmartParkingApp app;
    app.run();
    return 0;
}