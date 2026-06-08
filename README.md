# Smart Parking Management System

Proyek ini adalah implementasi sistem manajemen parkir cerdas berbasis C++. Proyek ini dirancang untuk memenuhi proyek akhir mata kuliah Algoritma dan Pemrograman.

## Fitur Utama

* [cite_start]**Manajemen Antrian (Queue):** Menggunakan *Linked List* kustom untuk menangani kendaraan masuk di gerbang secara FIFO (*First-In, First-Out*). [cite: 661]
* [cite_start]**Riwayat Parkir (Stack):** Menggunakan *Linked List* kustom untuk manajemen pembatalan (*undo*) transaksi keluar kendaraan (LIFO). [cite: 662]
* [cite_start]**Pencarian Efisien (Divide & Conquer):** Implementasi *Merge Sort* ($O(n \log n)$) dan *Binary Search* ($O(\log n)$) untuk pelacakan plat nomor secara cepat. [cite: 663]
* [cite_start]**Laporan Real-time (Multithreading):** Menggunakan `std::thread` dan `std::mutex` untuk menghasilkan statistik sistem secara paralel tanpa menyebabkan *bottleneck*. [cite: 664]
* [cite_start]**Defensive Programming:** Sistem validasi input yang tangguh untuk mencegah *error* atau *crash* pada *interface* baris perintah (CLI). [cite: 665]

## Persyaratan Sistem

* **Bahasa:** C++ (Standar C++17)
* **Compiler:** G++ atau *compiler* C++ yang mendukung standar C++17.
* [cite_start]**Library:** Native (Sesuai dengan ketentuan implementasi, *STL container* seperti `std::vector` tidak digunakan dalam algoritma utama). [cite: 667]
