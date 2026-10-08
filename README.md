# Snake Játék (OpenGL C++)

Egy egyedi, 2D-s Snake játék modern OpenGL (Core Profile) és C++ használatával[cite: 11]. A klasszikus játékmenetet olyan extra mechanikákkal egészíti ki, mint a pálya előzetes szerkesztése (akadályok lerakása), dinamikus színezés a pálya különböző zónáiban, valamint extra csapdák (kék kockák) és életrendszer (zöld kockák)[cite: 11].

## 🚀 Funkciók és Játékmenet

*   **Pályaszerkesztés (Pre-game):** A játék elindítása előtt a játékos egyedi akadályokat (fehér falakat) helyezhet el a pályán, vagy áthelyezheti a már lerakottakat[cite: 11].
*   **Dinamikus színezés:** A kígyó feje és teste dinamikusan változtatja a színét attól függően, hogy a képernyő melyik harmadában (bal, középső, jobb) tartózkodik[cite: 11].
*   **Tárgyak és pontozás:**
    *   🟥 **Piros kocka (Étel):** Növeli a kígyó hosszát és a pontszámot[cite: 11].
    *   🟦 **Kék kocka (Csapda):** Idővel véletlenszerűen jelennek meg. Ha a kígyó hozzáér, elveszít egy életet (maximum 6 kék kocka lehet a pályán egyszerre)[cite: 11].
    *   🟩 **Zöld kocka (Élet):** A képernyő jobb felső sarkában láthatóak, a játékos 3 élettel indul[cite: 11].
*   **Képernyő átlépése (Wrap-around):** Ha a kígyó kimegy a képernyő szélén, a túloldalon jelenik meg újra[cite: 11].
*   **Game Over feltételek:** A játéknak vége, ha a kígyó önmagába vagy egy lerakott akadályba (fehér fal) ütközik, esetleg elfogy az összes zöld élete[cite: 11].

## 🎮 Irányítás és Kezelés

| Gomb / Eszköz | Akció |
| :--- | :--- |
| **Bal egérgomb (Kattintás)** | Akadály (fal) lerakása a játék indítása előtt[cite: 11] |
| **Jobb egérgomb (Nyomva tart)** | Lerakott akadály megfogása és áthelyezése[cite: 11] |
| **Enter** | Játék elindítása (Szerkesztő módból való kilépés) / Újraindítás Game Over után[cite: 11] |
| **Nyílbillentyűk (Fel, Le, Balra, Jobbra)** | A kígyó irányítása[cite: 11] |
| **Esc** | Kilépés a játékból[cite: 11] |

## 🛠️ Használt Technológiák

*   **C++**
*   **OpenGL 4.0** (Core Profile)[cite: 11]
*   **GLFW:** Ablakkezelés és billentyűzet/egér bemenetek feldolgozása[cite: 11].
*   **GLAD:** OpenGL függvények betöltése[cite: 6, 11].
*   **GLM (OpenGL Mathematics):** Vektorok, mátrixok és transzformációk (pl. mozgás, távolságmérés) kezelése[cite: 11].

## ⚙️ Futtatás és Telepítés (Visual Studio)

1. Klónozd a tárolót a gépedre.
2. Győződj meg róla, hogy a `include` és a `lib-vc2022` mappák be vannak állítva, bennük a **GLFW**, **GLAD** és **GLM** könyvtárakkal[cite: 7].
3. A Visual Studio projektfájl (`.vcxproj`) használatával nyisd meg a projektet[cite: 7].
4. A futtatáshoz a lefordított `.exe` fájl mellett elérhetőnek kell lennie a `Shaders/` mappának, amely tartalmazza a következő shader fájlokat:
   *   `4.0.shader.vs` (Vertex Shader)[cite: 7, 8, 11]
   *   `4.0.shader.fs` (Fragment Shader)[cite: 7, 8, 11]

## 📁 Projekt Struktúra

*   `snake.cpp`: A fő játékciklust, a bemenetek kezelését (callback-ek), az ütközésvizsgálatot és a rajzolási logikát tartalmazó forrásfájl[cite: 11].
*   `shader.h`: Egyedi Shader osztály, amely beolvassa, lefordítja és linkeli a vertex és fragment shader kódokat, valamint kezeli a uniform változókat (pl. transzformációs mátrixok)[cite: 10].
*   `glad.c`: Az OpenGL kiterjesztéseket betöltő forrásfájl[cite: 6].
*   `Shaders/`: A grafikus kártyán futó GLSL shader programok mappája[cite: 7].