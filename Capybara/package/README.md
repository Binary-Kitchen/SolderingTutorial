# Capybara – C6-Umschlag

Druckvorlagen für den Versandumschlag des Capybara-Lötbausatzes: Vorder- und
Rückseite als Fertig-PDF plus die Affinity-Designer-Quelldateien zum Weiterbearbeiten.

## Dateien

| Datei | Inhalt |
|---|---|
| `capybara-c6-umschlag_v2.pdf` | **Vorderseite, druckfertig** (1 Seite) |
| `capybara-c6-umschlag_v2-rückseite.pdf` | **Rückseite, druckfertig** (1 Seite) |
| `capybara-c6-umschlag.afdesign` | Quelle Vorderseite (Affinity Designer 2) |
| `capybara-c6-umschlag-rückseite.afdesign` | Quelle Rückseite (Affinity Designer 2) |
| `batteryholder.avif` | Referenzfoto des verbauten Batteriehalters |

Die `*~lock~`-Dateien sind nur temporäre Sperrdateien von Affinity und gehören
nicht zur Gestaltung.

## Druckdaten

| | |
|---|---|
| Format | 162,28 × 114,30 mm (DIN C6 quer), randlos |
| Seiten | 2 Einzel-PDFs, je 1 Seite |
| Farben | **eine Farbe**: Schwarz auf blauem Papier |
| Schriften | Verdana (eingebettet, Subset) |
| Beschnitt | keiner – das Motiv läuft bis zur Kante |

Weiß kann nicht gedruckt werden. Alles, was im Design hell erscheint, ist
unbedrucktes Papier – die Blaufärbung des Umschlags scheint dort durch. Je heller
das Papier, desto besser der Kontrast; auf dunklem Marineblau ist die Gestaltung
kaum lesbar.

## Inhalt

**Vorderseite** – Titelzeile „CAPYBARA / SOLDER KIT", QR-Code zum Handbuch
(`https://binary-kitchen.github.io/SolderingTutorial/#Capybara`), Löt-Anleitung in
vier Schritten, Platinenansicht mit Pfeilen zu den Lötstellen, Callout
„bend LED legs", fertige Ansicht, Impressum.

1. **SWITCH** – bend the legs flat
2. **BATTERY HOLDER** – „+" to the mark
3. **3 × LED** – long leg = plus, short leg = minus
4. **CR2032** – insert into the holder, „+" facing up

**Rückseite** – Capybara-Illustration ohne Text.

## Bearbeiten

1. `.afdesign` in Affinity Designer 2 öffnen (oder in eine andere Version
   konvertieren); die Schrift **Verdana** muss installiert sein, sonst ersetzt
   Affinity sie.
2. Änderungen als PDF exportieren: Preset „PDF (press-ready)" bzw. PDF/X-4,
   Farbraum Graustufen bzw. 1 Sonderfarbe, Beschnitt 0 mm.
3. Beide Seiten getrennt exportieren – so sind die Dateien hier abgelegt.

Die Platinengeometrie in der Illustration stammt aus `../kicad/capybara.kicad_pcb`;
wer am Layout der Platine etwas ändert, muss die Zeichnung von Hand nachziehen.
