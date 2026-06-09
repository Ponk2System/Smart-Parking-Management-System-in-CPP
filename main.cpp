#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

struct Kendaraan {
    string platNomor;
    int entryHour;
    int exitHour;
    int biayaParkir;
};

// menggunakan Custom Dynamic Array pengganti std::vector)
class KendaraanArr {
private:
    Kendaraan* kendaraan;
    int kapasitas;
    int ukuran;

    void resize() {
        kapasitas *= 2;
        Kendaraan* newArr = new Kendaraan[kapasitas];
        for (int i = 0; i < ukuran; i++) {
            newArr[i] = kendaraan[i];
        }
        delete[] kendaraan;
        kendaraan = newArr;
    }

public:
    KendaraanArr() {
        kapasitas = 4;
        ukuran = 0;
        kendaraan = new Kendaraan[kapasitas];
    }

    ~KendaraanArr() { delete[] kendaraan; }

    KendaraanArr(const KendaraanArr& other) {
        kapasitas = other.kapasitas;
        ukuran = other.ukuran;
        kendaraan = new Kendaraan[kapasitas];
        for (int i = 0; i < ukuran; i++) kendaraan[i] = other.kendaraan[i];
    }

    KendaraanArr& operator=(const KendaraanArr& other) {
        if (this != &other) {
            delete[] kendaraan;
            kapasitas = other.kapasitas;
            ukuran = other.ukuran;
            kendaraan = new Kendaraan[kapasitas];
            for (int i = 0; i < ukuran; i++) kendaraan[i] = other.kendaraan[i];
        }
        return *this;
    }

    void push_back(Kendaraan v) {
        if (ukuran == kapasitas) resize();
        kendaraan[ukuran++] = v;
    }

    void hapus(int index) {
        if (index < 0 || index >= ukuran) return;
        for (int i = index; i < ukuran - 1; i++) {
            kendaraan[i] = kendaraan[i + 1];
        }
        ukuran--;
    }

    int getUkuran() const { return ukuran; }
    Kendaraan& operator[](int index) { return kendaraan[index]; }
    const Kendaraan& operator[](int index) const { return kendaraan[index]; }
};

// 1. Antrean (Queue - Linked List)
struct QNode {
    Kendaraan data;
    QNode* next;
    QNode(Kendaraan v) : data(v), next(nullptr) {}
};

class antreanParkir {
private:
    QNode* head;
    QNode* tail;
    int hitung;

public:
    antreanParkir() : head(nullptr), tail(nullptr), hitung(0) {}
    ~antreanParkir() { Kendaraan dummy; while (dequeue(dummy)); }
    
    void enqueue(string plate) {
        QNode* newNode = new QNode({plate, 0, 0, 0});
        if (tail == nullptr) head = tail = newNode;
        else {
        tail->next = newNode; tail = newNode;
        }
        hitung++;
    }

    bool dequeue(Kendaraan& v) {
        if (head == nullptr) return false;
        QNode* temp = head;
        v = temp->data;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete temp;
        hitung--;
        return true;
    }

    void display() {
        if (!head) { cout << "Antrean gerbang kosong.\n"; return; }
        QNode* temp = head;
        cout << "Antrean Masuk: ";
        while (temp) {
            cout << "[" << temp->data.platNomor << "] -> ";
            temp = temp->next;
        }
        cout << "NULL\n";
    }

    int getHitung() const { return hitung; }
};

// 2. Stack dan Linked List
struct Node {
    Kendaraan data;
    Node* next;
    Node(Kendaraan v) : data(v), next(nullptr) {}
};

class HistoryStack {
private:
    Node* top;
    int hitung;

public:
    HistoryStack() : top(nullptr), hitung(0) {}
    ~HistoryStack() { Kendaraan dummy; while (pop(dummy)); }

    void push(Kendaraan v) {
        Node* newNode = new Node(v);
        newNode->next = top;
        top = newNode;
        hitung++;
    }

    bool pop(Kendaraan& v) {
        if (top == nullptr) return false;
        Node* temp = top;
        v = temp->data;
        top = top->next;
        delete temp;
        hitung--;
        return true;
    }

    void displayTop() {
        if (top) cout << "Kendaraan terakhir keluar: " << top->data.platNomor << " (Biaya: Rp" << top->data.biayaParkir << ")\n";
        else cout << "Belum ada riwayat transaksi keluar.\n";
    }
    
    int getHitung() const { return hitung; }
    int getTotalPendapatan() {
        int total = 0;
        Node* temp = top;
        while (temp) { total += temp->data.biayaParkir; temp = temp->next; }
        return total;
    }
};

// 3. Searching (Divide & Conquer)
class dbParkir {
public:
    KendaraanArr sudahParkir;

    void merge(KendaraanArr& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        Kendaraan* L = new Kendaraan[n1];
        Kendaraan* R = new Kendaraan[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].platNomor <= R[j].platNomor) { arr[k] = L[i]; i++; } 
            else { arr[k] = R[j]; j++; }
            k++;
        }
        while (i < n1) { arr[k] = L[i]; i++; k++; }
        while (j < n2) { arr[k] = R[j]; j++; k++; }

        delete[] L;
        delete[] R;
    }

    void mergeSort(KendaraanArr& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    int binarySearch(const KendaraanArr& arr, int left, int right, const string& target) {
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid].platNomor == target) return mid;
            if (arr[mid].platNomor < target) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
};

class SPMS {
private:
    antreanParkir queue;
    HistoryStack history;
    dbParkir db;
    mutex mtx;
    const int TARIF_PER_JAM = 3000;

    int hitungBiaya(int masuk, int keluar) {
        int durasi = keluar - masuk;
        if (durasi < 0) durasi += 24;
        if (durasi == 0) durasi = 1; 
        return durasi * TARIF_PER_JAM;
    }

    // Fungsi Validasi
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
            } 
            else {
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
            } 
            else {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[ERROR] Input harus berupa angka.\n";
            }
            cout << "Masukkan kembali Jam (0-23): ";
        }
    }

public:
    void menuTambahAntrean() {
        cout << "\n--- TAMBAH ANTREAN GERBANG ---\n";
        cout << "Masukkan Plat Nomor (Tanpa Spasi): ";
        string plat = inputPlatValid();
        
        queue.enqueue(plat); 
        cout << "[QUEUE] Kendaraan " << plat << " masuk antrEan gerbang." << endl;
    }

    void menuParkirkan() {
        Kendaraan v;
        if (queue.dequeue(v)) {
            cout << "Kendaraan [" << v.platNomor << "] dipanggil ke slot parkir." << endl;
            cout << "Input Jam Masuk (Format 0-23): ";
            int jamMasuk = inputJamValid();
            
            v.entryHour = jamMasuk;
            db.sudahParkir.push_back(v);
            cout << "[PARKIR] " << v.platNomor << " resmi menempati slot parkir." << endl;
        } 
        else {
            cout << "[ERROR] Tidak ada kendaraan di antrean gerbang!" << endl;
        }
    }

    void menuKeluarkan() {
        if (db.sudahParkir.getUkuran() == 0) {
            cout << "Area parkir kosong. Tidak ada kendaraan untuk dikeluarkan." << endl;
            return;
        }

        cout << "\n=== DAFTAR KENDARAAN AKTIF PARKIR ===\n";
        for (int i = 0; i < db.sudahParkir.getUkuran(); i++) {
            cout << i + 1 << ". Plat: " << db.sudahParkir[i].platNomor 
                 << " | Jam Masuk: " << db.sudahParkir[i].entryHour << "\n";
        }

        string plat;
        int indexKendaraan = -1;

        while (true) {
            cout << "Masukkan Plat Kendaraan yang Akan Keluar (Sesuai Daftar): ";
            plat = inputPlatValid();

            for (int i = 0; i < db.sudahParkir.getUkuran(); i++) {
                if (db.sudahParkir[i].platNomor == plat) {
                    indexKendaraan = i;
                    break;
                }
            }
            if (indexKendaraan != -1) break; 
            else cout << "[ERROR] Plat " << plat << " tidak ditemukan!\n";
        }

        cout << "[DITEMUKAN] Jam masuk tercatat: " << db.sudahParkir[indexKendaraan].entryHour << "\n";
        cout << "Masukkan Jam Keluar (0-23): ";
        int jamKeluar = inputJamValid();

        db.sudahParkir[indexKendaraan].exitHour = jamKeluar;
        db.sudahParkir[indexKendaraan].biayaParkir = hitungBiaya(db.sudahParkir[indexKendaraan].entryHour, jamKeluar);

        cout << "[TRANSAKSI] Durasi Terhitung. Total Biaya: Rp" << db.sudahParkir[indexKendaraan].biayaParkir << "\n";
        
        history.push(db.sudahParkir[indexKendaraan]); 
        db.sudahParkir.hapus(indexKendaraan);
        cout << "[KELUAR SUCCESS] Kendaraan " << plat << " telah keluar." << endl;
    }

    void Undo() {
        Kendaraan v;
        if (history.pop(v)) {
            v.exitHour = 0;
            v.biayaParkir = 0;
            db.sudahParkir.push_back(v);
            cout << "[UNDO SUCCESS] " << v.platNomor << " dikembalikan ke area parkir." << endl;
        } 
        else {
            cout << "[ERROR] Tidak ada riwayat transaksi." << endl;
        }
    }

    void Pencarian() {
        if (db.sudahParkir.getUkuran() == 0) { 
            cout << "Area parkir kosong.\n"; 
            return; 
        }

        cout << "\n=== DAFTAR PLAT KENDARAAN AKTIF ===\n";
        for (int i = 0; i < db.sudahParkir.getUkuran(); i++) {
            cout << "- " << db.sudahParkir[i].platNomor << "\n";
        }
        cout << "===================================\n";
        
        cout << "Masukkan Plat Nomor yang dicari: ";
        string plat = inputPlatValid();

        KendaraanArr tempArr = db.sudahParkir;
        db.mergeSort(tempArr, 0, tempArr.getUkuran() - 1);
        int idx = db.binarySearch(tempArr, 0, tempArr.getUkuran() - 1, plat);

        if (idx != -1) {
            cout << "[FOUND] Plat Nomor " << plat << " ditemukan! Kendaraan aktif parkir sejak jam " << tempArr[idx].entryHour << ".\n";
        } 
        else {
            cout << "[NOT FOUND] Plat Nomor " << plat << " tidak ditemukan di area parkir.\n";
        }
    }

    // --- MULTITHREADING FUNGSI ---
    void HitungAntrean(int& res) { this_thread::sleep_for(chrono::milliseconds(200)); lock_guard<mutex> lock(mtx); res = queue.getHitung(); }
    void HitungParkir(int& res) { this_thread::sleep_for(chrono::milliseconds(250)); lock_guard<mutex> lock(mtx); res = db.sudahParkir.getUkuran(); }
    void HitungKeluar(int& res) { this_thread::sleep_for(chrono::milliseconds(150)); lock_guard<mutex> lock(mtx); res = history.getHitung(); }
    void HitungPendapatan(int& res) { this_thread::sleep_for(chrono::milliseconds(300)); lock_guard<mutex> lock(mtx); res = history.getTotalPendapatan(); }

    void Laporan() {
        cout << "\n=== MEMPROSES DASHBOARD STATISTIK ===\n";
        int tQueue = 0, tParkir = 0, tKeluar = 0, tRevenue = 0;
        auto start = chrono::high_resolution_clock::now();

        thread th1(&SPMS::HitungAntrean, this, ref(tQueue));
        thread th2(&SPMS::HitungParkir, this, ref(tParkir));
        thread th3(&SPMS::HitungKeluar, this, ref(tKeluar));
        thread th4(&SPMS::HitungPendapatan, this, ref(tRevenue));

        th1.join(); th2.join(); th3.join(); th4.join();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;

        cout << " Kendaraan Mengantre  : " << tQueue << " unit\n";
        cout << " Kendaraan Aktif      : " << tParkir << " unit\n";
        cout << " Kendaraan Selesai    : " << tKeluar << " unit\n";
        cout << " Total Pendapatan     : Rp" << tRevenue << "\n";
        cout << " Latency              : " << duration.count() << " ms\n";
    }

    void run() {
        int pilihan = -1;
        while (pilihan != 0) {
            cout << "\n=== SMART PARKING MANAGEMENT ===\n";
            cout << "1. Tambah Antrean Masuk\n";
            cout << "2. Panggil Antrean ke Area Parkir\n";
            cout << "3. Proses Kendaraan Keluar & Hitung Biaya\n";
            cout << "4. Undo Transaksi Keluar Terakhir\n";
            cout << "5. Pencarian Kendaraan Aktif\n";
            cout << "6. Tampilkan Laporan Omset\n";
            cout << "7. Monitoring Log Antrean & Histori Teratas\n";
            cout << "0. Keluar Aplikasi\n";
            cout << "Pilih menu: ";
            
            if (!(cin >> pilihan)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[ERROR] Input tidak valid! Harap masukkan angka menu.\n";
                continue;
            }

            switch (pilihan) {
                case 1: menuTambahAntrean(); break;
                case 2: menuParkirkan(); break;
                case 3: menuKeluarkan(); break;
                case 4: Undo(); break;
                case 5: Pencarian(); break;
                case 6: Laporan(); break;
                case 7: queue.display(); history.displayTop(); break;
                case 0: cout << "Sistem dimatikan." << endl; break;
                default: cout << "[ERROR] Menu tidak tersedia!" << endl;
            }
        }
    }
};

int main() {
    SPMS app;
    app.run();
    return 0;
}
