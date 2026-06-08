# Smart Parking Management System

Proyek ini adalah implementasi sistem manajemen parkir cerdas berbasis C++. Proyek ini dirancang untuk memenuhi proyek akhir mata kuliah Algoritma dan Pemrograman.

## Fitur Utama

* **Manajemen Antrian (Queue):** Menggunakan *Linked List* kustom untuk menangani kendaraan masuk di gerbang secara FIFO (*First-In, First-Out*). 
* **Riwayat Parkir (Stack):** Menggunakan *Linked List* kustom untuk manajemen pembatalan (*undo*) transaksi keluar kendaraan (LIFO). 
* **Pencarian Efisien (Divide & Conquer):** Implementasi *Merge Sort* ($O(n \log n)$) dan *Binary Search* ($O(\log n)$) untuk pelacakan plat nomor secara cepat. 
* **Laporan Real-time (Multithreading):** Menggunakan `std::thread` dan `std::mutex` untuk menghasilkan statistik sistem secara paralel tanpa menyebabkan *bottleneck*. 
* **Defensive Programming:** Sistem validasi input yang tangguh untuk mencegah *error* atau *crash* pada *interface* baris perintah (CLI). 

## Persyaratan

* **Library:** Native (Sesuai dengan ketentuan implementasi, *STL container* seperti `std::vector` tidak digunakan dalam algoritma utama). 
