# Snake Játék (OpenGL C++)

Egy egyedi, 2D-s Snake játék modern OpenGL (Core Profile) és C++ használatával. A klasszikus játékmenetet olyan extra mechanikákkal egészíti ki, mint a pálya előzetes szerkesztése (akadályok lerakása), dinamikus színezés a pálya különböző zónáiban, valamint extra csapdák (kék kockák) és életrendszer (zöld kockák).

## 🚀 Funkciók és Játékmenet

*   **Pályaszerkesztés (Pre-game):** A játék elindítása előtt a játékos egyedi akadályokat (fehér falakat) helyezhet el a pályán, vagy áthelyezheti a már lerakottakat.
*   **Dinamikus színezés:** A kígyó feje és teste dinamikusan változtatja a színét attól függően, hogy a képernyő melyik harmadában (bal, középső, jobb) tartózkodik.
*   **Tárgyak és pontozás:**
    *   🟥 **Piros kocka (Étel):** Növeli a kígyó hosszát és a pontszámot[cite: 11].
    *   🟦 **Kék kocka (Csapda):** Idővel véletlenszerűen jelennek meg. Ha a kígyó hozzáér, elveszít egy életet (maximum 6 kék kocka lehet a pályán egyszerre).
    *   🟩 **Zöld kocka (Élet):** A képernyő jobb felső sarkában láthatóak, a játékos 3 élettel indul.
*   **Képernyő átlépése (Wrap-around):** Ha a kígyó kimegy a képernyő szélén, a túloldalon jelenik meg újra.
*   **Game Over feltételek:** A játéknak vége, ha a kígyó önmagába vagy egy lerakott akadályba (fehér fal) ütközik, esetleg elfogy az összes zöld élete.

## 🎮 Irányítás és Kezelés

| Gomb / Eszköz | Akció |
| :--- | :--- |
| **Bal egérgomb (Kattintás)** | Akadály (fal) lerakása a játék indítása előtt |
| **Jobb egérgomb (Nyomva tart)** | Lerakott akadály megfogása és áthelyezése |
| **Enter** | Játék elindítása (Szerkesztő módból való kilépés) / Újraindítás Game Over után |
| **Nyílbillentyűk (Fel, Le, Balra, Jobbra)** | A kígyó irányítása |
| **Esc** | Kilépés a játékból |

## 🛠️ Használt Technológiák

*   **C++**
*   **OpenGL 4.0** (Core Profile)
*   **GLFW:** Ablakkezelés és billentyűzet/egér bemenetek feldolgozása.
*   **GLAD:** OpenGL függvények betöltése.
*   **GLM (OpenGL Mathematics):** Vektorok, mátrixok és transzformációk (pl. mozgás, távolságmérés) kezelése.


## 📁 Projekt Struktúra

*   `snake.cpp`: A fő játékciklust, a bemenetek kezelését (callback-ek), az ütközésvizsgálatot és a rajzolási logikát tartalmazó forrásfájl.
*   `shader.h`: Egyedi Shader osztály, amely beolvassa, lefordítja és linkeli a vertex és fragment shader kódokat, valamint kezeli a uniform változókat (pl. transzformációs mátrixok.
*   `glad.c`: Az OpenGL kiterjesztéseket betöltő forrásfájl.
*   `Shaders/`: A grafikus kártyán futó GLSL shader programok mappája.
