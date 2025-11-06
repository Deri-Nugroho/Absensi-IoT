const API_URL = "https://YOUR_API_GATEWAY_URL/siswa"; // Ganti URL API kamu

async function getAbsen() {
  try {
    const res = await fetch(API_URL);
    if (!res.ok) throw new Error(`❌ Gagal GET: ${res.status}`);
    const data = await res.json();

    const list = document.getElementById("daftarAbsen");
    list.innerHTML = "";
    data.forEach(item => {
      const li = document.createElement("li");
      li.textContent = `${item.nama} - ${item.pesan}`;
      list.appendChild(li);
    });
    console.log("✅ Data berhasil diambil:", data);
  } catch (err) {
    console.error("❌ ERROR GET:", err);
  }
}

document.getElementById("absenForm").addEventListener("submit", async e => {
  e.preventDefault();
  const nama = document.getElementById("nama").value;
  try {
    const res = await fetch(API_URL, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ nama })
    });
    if (!res.ok) throw new Error(`❌ Gagal POST: ${res.status}`);
    console.log("✅ Data absen dikirim:", nama);
    document.getElementById("nama").value = "";
    getAbsen();
  } catch (err) {
    console.error("❌ ERROR POST:", err);
    alert("❌ Gagal mengirim data absen!");
  }
});

getAbsen();
setInterval(getAbsen, 10000);
