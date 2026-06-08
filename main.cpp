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

    ~VehicleArray() { delete[] arr; }

    VehicleArray(const VehicleArray& other) {
        capacity = other.capacity;
        size = other.size;
        arr = new Vehicle[capacity];
        for (int i = 0; i < size; i++) arr[i] = other.arr[i];
    }

    VehicleArray& operator=(const VehicleArray& other) {
        if (this != &other) {
            delete[] arr;
            capacity = other.capacity;
            size = other.size;
            arr = new Vehicle[capacity];
            for (int i = 0; i < size; i++) arr[i] = other.arr[i];
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
    ~ParkingQueue() { Vehicle dummy; while (dequeue(dummy)); }
    
    void enqueue(string plate) {
        QNode* newNode = new QNode({plate, 0, 0, 0});
        if (tail == nullptr) head = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        count++;
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
    ~HistoryStack() { Vehicle dummy; while (pop(dummy)); }

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
        if (top) cout << "Kendaraan terakhir keluar: " << top->data.plateNumber << " (Biaya: Rp" << top->data.parkingFee << ")\n";
        else cout << "Belum ada riwayat transaksi keluar.\n";
    }
    
    int getCount() const { return count; }
    int getTotalRevenue() {
        int total = 0;
        SNode* temp = top;
        while (temp) { total += temp->data.parkingFee; temp = temp->next; }
        return total;
    }
};

// ==========================================
// 3. MODUL PENCARIAN (DIVIDE & CONQUER)
// ==========================================
class ParkingDatabase {
public:
    VehicleArray parkedVehicles;

    void merge(VehicleArray& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        Vehicle* L = new Vehicle[n1];
        Vehicle* R = new Vehicle[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].plateNumber <= R[j].plateNumber) { arr[k] = L[i]; i++; } 
            else { arr[k] = R[j]; j++; }
            k++;
        }
        while (i < n1) { arr[k] = L[i]; i++; k++; }
        while (j < n2) { arr[k] = R[j]; j++; k++; }

        delete[] L;
        delete[] R;
    }

    void mergeSort(VehicleArray& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

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
// 4. APLIKASI UTAMA & LOGIKA SISTEM
// ==========================================
class SmartParkingApp {
private:
    ParkingQueue queue;
    HistoryStack history;
    ParkingDatabase db;
    mutex mtx;
    const int TARIF_PER_JAM = 3000;

    int hitungBiaya(int masuk, int keluar) {
        int durasi = keluar - masuk;
        if (durasi < 0) durasi += 24;
        if (durasi == 0) durasi = 1; 
        return durasi * TARIF_PER_JAM;
    }

    // --- FUNGSI VALIDASI ---
    string inputPlatValid() {
        string plat;
        while (true) {
            getline(cin >> ws, plat);
            bool adaSpasi = false;
            for (int i = 0; i < plat.length(); i++) {
                if (plat[i] == ' ') {
                    adaSpasi = true;
                    break;
                }
            }
            if (adaSpasi) {
                cout << "[ERROR] Plat nomor tidak boleh ada spasi! (Contoh: AA17)\n";
                cout << "Masukkan kembali Plat Nomor: ";
            } else {
                return plat;
            }
        }
    }

    int inputJamValid() {
        int jam;
        while (true) {
            if (cin >> jam) {
                if (jam >= 0 && jam <= 23) return jam;
                else cout << "[ERROR] Jam harus antara 0 - 23.\n";
            } else {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[ERROR] Input harus berupa angka.\n";
            }
            cout << "Masukkan kembali Jam (0-23): ";
        }
    }

public:
    void menuTambahAntrian() {
        cout << "\n--- TAMBAH ANTRIAN GERBANG ---\n";
        cout << "Masukkan Plat Nomor (Tanpa Spasi): ";
        string plat = inputPlatValid();
        
        queue.enqueue(plat); 
        cout << "[QUEUE] Kendaraan " << plat << " masuk antrian gerbang.\n";
    }

    void menuParkirkan() {
        Vehicle v;
        if (queue.dequeue(v)) {
            cout << "Kendaraan [" << v.plateNumber << "] dipanggil ke slot parkir.\n";
            cout << "Input Jam Masuk (Format 0-23): ";
            int jamMasuk = inputJamValid(); // Menggunakan fungsi validasi agar aman dari input huruf
            
            v.entryHour = jamMasuk;
            db.parkedVehicles.push_back(v);
            cout << "[PARKIR] " << v.plateNumber << " resmi menempati slot parkir.\n";
        } else {
            cout << "[ERROR] Tidak ada kendaraan di antrian gerbang!\n";
        }
    }

    void menuKeluarkan() {
        if (db.parkedVehicles.getSize() == 0) {
            cout << "Area parkir kosong. Tidak ada kendaraan untuk dikeluarkan.\n";
            return;
        }

        cout << "\n=== DAFTAR KENDARAAN AKTIF PARKIR ===\n";
        for (int i = 0; i < db.parkedVehicles.getSize(); i++) {
            cout << i + 1 << ". Plat: " << db.parkedVehicles[i].plateNumber 
                 << " | Jam Masuk: " << db.parkedVehicles[i].entryHour << "\n";
        }

        string plat;
        int indexKendaraan = -1;

        while (true) {
            cout << "Masukkan Plat Kendaraan yang Akan Keluar (Sesuai Daftar): ";
            plat = inputPlatValid();

            for (int i = 0; i < db.parkedVehicles.getSize(); i++) {
                if (db.parkedVehicles[i].plateNumber == plat) {
                    indexKendaraan = i;
                    break;
                }
            }
            if (indexKendaraan != -1) break; 
            else cout << "[ERROR] Plat " << plat << " tidak ditemukan!\n";
        }

        cout << "[DITEMUKAN] Jam masuk tercatat: " << db.parkedVehicles[indexKendaraan].entryHour << "\n";
        cout << "Masukkan Jam Keluar (0-23): ";
        int jamKeluar = inputJamValid();

        db.parkedVehicles[indexKendaraan].exitHour = jamKeluar;
        db.parkedVehicles[indexKendaraan].parkingFee = hitungBiaya(db.parkedVehicles[indexKendaraan].entryHour, jamKeluar);

        cout << "[TRANSAKSI] Durasi Terhitung. Total Biaya: Rp" << db.parkedVehicles[indexKendaraan].parkingFee << "\n";
        
        history.push(db.parkedVehicles[indexKendaraan]); 
        db.parkedVehicles.erase(indexKendaraan);
        cout << "[KELUAR SUCCESS] Kendaraan " << plat << " telah keluar.\n";
    }

    void menuUndo() {
        Vehicle v;
        if (history.pop(v)) {
            v.exitHour = 0;
            v.parkingFee = 0;
            db.parkedVehicles.push_back(v);
            cout << "[UNDO SUCCESS] " << v.plateNumber << " dikembalikan ke area parkir.\n";
        } else {
            cout << "[ERROR] Tidak ada riwayat transaksi.\n";
        }
    }

    void menuCari() {
        if (db.parkedVehicles.getSize() == 0) { 
            cout << "Area parkir kosong.\n"; 
            return; 
        }

        // 1. Menampilkan list plat nomor kendaraan aktif
        cout << "\n=== DAFTAR PLAT KENDARAAN AKTIF ===\n";
        for (int i = 0; i < db.parkedVehicles.getSize(); i++) {
            cout << "- " << db.parkedVehicles[i].plateNumber << "\n";
        }
        cout << "===================================\n";
        
        // 2. Meminta input plat dari user
        cout << "Masukkan Plat Nomor yang dicari: ";
        string plat = inputPlatValid();

        VehicleArray tempArr = db.parkedVehicles;
        // Divide and Conquer: Merge Sort
        db.mergeSort(tempArr, 0, tempArr.getSize() - 1);
        // Divide and Conquer: Binary Search
        int idx = db.binarySearch(tempArr, 0, tempArr.getSize() - 1, plat);

        // 3. Konfirmasi plat ditemukan beserta jam masuk
        if (idx != -1) {
            cout << "[FOUND] Plat Nomor " << plat << " ditemukan! Kendaraan aktif parkir sejak jam " << tempArr[idx].entryHour << ".\n";
        } else {
            cout << "[NOT FOUND] Plat Nomor " << plat << " tidak ditemukan di area parkir.\n";
        }
    }

    // --- MULTITHREADING FUNGSI ---
    void taskHitungAntrian(int& res) { this_thread::sleep_for(chrono::milliseconds(200)); lock_guard<mutex> lock(mtx); res = queue.getCount(); }
    void taskHitungParkir(int& res) { this_thread::sleep_for(chrono::milliseconds(250)); lock_guard<mutex> lock(mtx); res = db.parkedVehicles.getSize(); }
    void taskHitungKeluar(int& res) { this_thread::sleep_for(chrono::milliseconds(150)); lock_guard<mutex> lock(mtx); res = history.getCount(); }
    void taskHitungPendapatan(int& res) { this_thread::sleep_for(chrono::milliseconds(300)); lock_guard<mutex> lock(mtx); res = history.getTotalRevenue(); }

    void menuLaporan() {
        cout << "\n=== MEMPROSES DASHBOARD STATISTIK ===\n";
        int tQueue = 0, tParkir = 0, tKeluar = 0, tRevenue = 0;
        auto start = chrono::high_resolution_clock::now();

        thread th1(&SmartParkingApp::taskHitungAntrian, this, ref(tQueue));
        thread th2(&SmartParkingApp::taskHitungParkir, this, ref(tParkir));
        thread th3(&SmartParkingApp::taskHitungKeluar, this, ref(tKeluar));
        thread th4(&SmartParkingApp::taskHitungPendapatan, this, ref(tRevenue));

        th1.join(); th2.join(); th3.join(); th4.join();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;

        cout << " Kendaraan Mengantri  : " << tQueue << " unit\n";
        cout << " Kendaraan Aktif      : " << tParkir << " unit\n";
        cout << " Kendaraan Selesai    : " << tKeluar << " unit\n";
        cout << " Total Pendapatan     : Rp" << tRevenue << "\n";
        cout << " Latency              : " << duration.count() << " ms\n";
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
            
            if (!(cin >> pilihan)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[ERROR] Input tidak valid! Harap masukkan angka menu.\n";
                continue;
            }

            switch (pilihan) {
                case 1: menuTambahAntrian(); break;
                case 2: menuParkirkan(); break;
                case 3: menuKeluarkan(); break;
                case 4: menuUndo(); break;
                case 5: menuCari(); break;
                case 6: menuLaporan(); break;
                case 7: queue.display(); history.displayTop(); break;
                case 0: cout << "Sistem dimatikan.\n"; break;
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
