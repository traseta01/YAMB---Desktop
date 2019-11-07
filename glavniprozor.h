#ifndef GLAVNIPROZOR_H
#define GLAVNIPROZOR_H

#include <QMainWindow>

namespace Ui {
class GlavniProzor;
}

class GlavniProzor : public QMainWindow
{
    Q_OBJECT

public:
    explicit GlavniProzor(QWidget *parent = 0);
    ~GlavniProzor();
	void PoljeZaUpisOslobodi();

private:
    Ui::GlavniProzor *ui;

private slots:
	void DugmeBaciKliknuto();
	
	void NovaIgraKliknuto();
	
	void PoljeZaUpisDoleKliknuto();

	void PoljeZaUpisSlobKliknuto();

	void PoljeZaUpisGoreKliknuto();

	void PoljeZaUpisNajaKliknuto();
	
	void VratiKockicu();

	void IzaberiKockicu();
	
	void IzracunajKliknuto();
};

#endif // GLAVNIPROZOR_H
