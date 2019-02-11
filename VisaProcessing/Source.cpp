#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <exception>
using namespace std;
mutex strazar;
char *crt = "\n-----------------------------------------------\n";

enum Status { ZAHTJEV_ZAPRIMLJEN, ODUSTAO, ODBIJENA, IZDATA, ISTEKLA, PONISTENA };
char *_statusVize[]{ "ZAHTJEV_ZAPRIMLJEN", "ODUSTAO", "ODBIJENA", "IZDATA", "ISTEKLA", "PONISTENA" };
enum Sortiranje { RASTUCI, OPADAJUCI };
char *_vrstaSortiranja[]{ "RASTUCI", "OPADAJUCI" };
enum Drzava { SAD, AUSTRALIJA, ENGLESKA };
char *_ispisDrzave[]{ "SAD", "AUSTRALIJA", "ENGLESKA" };

bool PrestupnaGodina(int godina) {
	return ((godina % 400 == 0 || godina % 100 != 0) && godina % 4 == 0);
}

class Datum {
	int *_dan, *_mjesec, *_godina;
public:
	Datum(int dan = 1, int mjesec = 1, int godina = 2000) {
		_dan = new int(dan);
		_mjesec = new int(mjesec);
		_godina = new int(godina);
	}
	~Datum() {
		delete _dan; _dan = nullptr;
		delete _mjesec; _mjesec = nullptr;
		delete _godina; _godina = nullptr;
	}
	Datum(const Datum &obj) {
		_dan = new int(*obj._dan);
		_mjesec = new int(*obj._mjesec);
		_godina = new int(*obj._godina);
	}
	Datum & operator = (const Datum &obj) {
		if (this != &obj) {
			delete _dan; delete _mjesec; delete _godina;
			_dan = new int(*obj._dan);
			_mjesec = new int(*obj._mjesec);
			_godina = new int(*obj._godina);
		} return *this;
	}
	int sumaDana() {
		int brDana[]{ 31,28 + PrestupnaGodina(*_godina), 31,30,31,30,31,31,30,31,30,31 }, suma = *_dan;

		for (int i = 1950; i < *_godina; i++)
			suma += PrestupnaGodina(i) ? 366 : 365;

		for (int i = 1; i < *_mjesec; i++)
			suma += brDana[i - 1];

		return suma;
	}
	Datum & operator ++ () {
		int brDana[]{ 31,28 + PrestupnaGodina(*_godina), 31,30,31,30,31,31,30,31,30,31 };

		*_dan = (*_dan % brDana[*_mjesec - 1]) + 1;
		*_mjesec = (*_dan == 1) ? *_mjesec + 1 == 13 ? 1 : *_mjesec + 1 : *_mjesec;
		*_godina = (*_mjesec == 1 && *_dan == 1) ? *_godina + 1 : *_godina;

		return *this;
	}
	int GodineStarosti(int dan = 26, int mjesec = 6, int godina = 2017) {
		int godineStarosti = godina - *_godina;
		godineStarosti = (*_mjesec > mjesec) ? godineStarosti - 1 : *_mjesec == mjesec ? *_dan < dan ? godineStarosti - 1 : godineStarosti : godineStarosti;
		return godineStarosti;
	}
	friend ostream& operator<< (ostream &COUT, const Datum &obj);
	friend bool operator == (const Datum &d1, const Datum &d2);
	friend bool operator < (const Datum &d1, const Datum &d2);
	friend bool operator > (const Datum &d1, const Datum &d2);
	friend bool operator <= (const Datum &d1, const Datum &d2);
	friend bool operator >= (const Datum &d1, const Datum &d2);
};
ostream& operator<< (ostream &COUT, const Datum &obj) {
	COUT << *obj._dan << "." << *obj._mjesec << "." << *obj._godina;
	return COUT;
}
bool operator == (const Datum &d1, const Datum &d2) {
	return *d1._godina == *d2._godina && *d1._mjesec == *d2._mjesec && *d1._dan == *d2._dan;
}
bool operator < (const Datum &d1, const Datum &d2) {
	if (*d1._godina < *d2._godina)
		return true;
	if (*d1._godina == *d2._godina) {
		if (*d1._mjesec < *d2._mjesec)
			return true;
		if (*d1._mjesec == *d2._mjesec) {
			if (*d1._dan < *d2._dan)
				return true;
		}
	} return false;
}
bool operator > (const Datum &d1, const Datum &d2) {
	return !(d1 < d2) && !(d1 == d2);
}
bool operator <= (const Datum &d1, const Datum &d2) {
	return d1 < d2 || d1 == d2;
}
bool operator >= (const Datum &d1, const Datum &d2) {
	return d1 > d2 || d1 == d2;
}

template<class T1>
void SWAP(T1 &e1, T1 &e2) {
	T1 temp = e1;
	e1 = e2;
	e2 = temp;
}

template<class T1, class T2, int max>
class FITKolekcija {
	T1 * _elementi1[max];
	T2 * _elementi2[max];
	int _trenutno;
public:
	FITKolekcija() {
		for (size_t i = 0; i < max; i++) {
			_elementi1[i] = nullptr;
			_elementi2[i] = nullptr;
		}
		_trenutno = 0;
	}
	~FITKolekcija() {
		for (size_t i = 0; i < _trenutno; i++) {
			delete _elementi1[i]; _elementi1[i] = nullptr;
			delete _elementi2[i]; _elementi2[i] = nullptr;
		}
	}
	FITKolekcija(const FITKolekcija &obj) {
		for (int i = 0; i < max; i++) {
			if (i < obj._trenutno) {
				_elementi1[i] = new T1(*obj._elementi1[i]);
				_elementi2[i] = new T2(*obj._elementi2[i]);
			}
			else {
				_elementi1[i] = nullptr;
				_elementi2[i] = nullptr;
			}
		}
	}
	FITKolekcija & operator = (const FITKolekcija &obj) {
		if (this != &obj) {
			for (int i = 0; i < _trenutno; i++) {
				delete _elementi1[i];
				delete _elementi2[i];
			}
			for (int i = 0; i < max; i++) {
				if (i < obj._trenutno) {
					_elementi1[i] = new T1(*obj._elementi1[i]);
					_elementi2[i] = new T2(*obj._elementi2[i]);
				}
				else {
					_elementi1[i] = nullptr;
					_elementi2[i] = nullptr;
				}
			}
		} return *this;
	}
	void Dodaj(T1 e1, T2 e2) {
		if (_trenutno == max) throw exception("Prekoracenje opsega niza!");
		_elementi1[_trenutno] = new T1(e1);
		_elementi2[_trenutno++] = new T2(e2);
	}
	void Sortiraj(Sortiranje tip) {
		if (tip == OPADAJUCI) {
			bool promjena = true; int j = 0;
			while (promjena) {
				promjena = false;
				j++;
				for (int i = 0; i < _trenutno - j; i++) {
					if (*_elementi1[i] < *_elementi1[i + 1]) {
						SWAP(_elementi1[i], _elementi1[i + 1]);
						SWAP(_elementi2[i], _elementi2[i + 1]);
						promjena = true;
					}
				}
			}
		}
		else {
			bool promjena = true; int j = 0;
			while (promjena) {
				promjena = false;
				j++;
				for (int i = 0; i < _trenutno - j; i++) {
					if (*_elementi1[i] > *_elementi1[i + 1]) {
						SWAP(_elementi1[i], _elementi1[i + 1]);
						SWAP(_elementi2[i], _elementi2[i + 1]);
						promjena = true;
					}
				}
			}
		}
	}
	T1 ** GetT1() { return _elementi1; }
	T2 ** GetT2() { return _elementi2; }
	int GetTrenutno() { return _trenutno; }

	friend ostream& operator<< <> (ostream &COUT, FITKolekcija<T1, T2, max> &obj);
};
template<class T1, class T2, int max>
ostream& operator<< (ostream &COUT, FITKolekcija<T1, T2, max> &obj) {
	for (size_t i = 0; i < obj._trenutno; i++)
		COUT << *obj._elementi1[i] << " " << *obj._elementi2[i] << endl;
	return COUT;
}

class Viza {
protected:
	Drzava _drzava;
	Datum _vazenjeOD;
	Datum _vazenjeDO;
	char * _napomena;
	// string se odnosi na razlog promjene statusa, a moguæe je samo 10 izmjena statusa
	FITKolekcija<Status, string, 10> _statusi;
public:
	Viza(Drzava drzava, Datum _OD, Datum _DO, char * napomena = nullptr) :_vazenjeDO(_DO), _vazenjeOD(_OD)
	{
		if (napomena != nullptr) {
			_napomena = new char[strlen(napomena) + 1];
			strcpy_s(_napomena, strlen(napomena) + 1, napomena);
		}
		else
			_napomena = nullptr;
		_drzava = drzava;
	}
	~Viza() { delete[] _napomena; _napomena = nullptr; }
	Viza(const Viza &obj) : _vazenjeDO(obj._vazenjeDO), _vazenjeOD(obj._vazenjeOD) {
		if (obj._napomena != nullptr) {
			_napomena = new char[strlen(obj._napomena) + 1];
			strcpy_s(_napomena, strlen(obj._napomena) + 1, obj._napomena);
		}
		else
			_napomena = nullptr;
		_drzava = obj._drzava;
	}
	Viza & operator = (const Viza &obj) {
		if (this != &obj) {
			delete[]_napomena;
			if (obj._napomena != nullptr) {
				_napomena = new char[strlen(obj._napomena) + 1];
				strcpy_s(_napomena, strlen(obj._napomena) + 1, obj._napomena);
			}
			else
				_napomena = nullptr;
			_vazenjeDO = obj._vazenjeDO;
			_vazenjeOD = obj._vazenjeOD;
			_drzava = obj._drzava;
		} return *this;
	}
	Datum GetVazenjeOD() { return _vazenjeOD; }
	Datum GetVazenjeDO() { return _vazenjeDO; }
	char * GetNapomena() { return (_napomena == nullptr ? "" : _napomena); }
	Drzava GetDrzava() { return _drzava; }
	FITKolekcija<Status, string, 10> & GetStatuse() { return _statusi; }
	virtual void Info() = 0;
	friend bool operator == (Viza &prva, Viza &druga);
	friend ostream & operator << (ostream &COUT, Viza &viza);
};
bool operator == (Viza &prva, Viza &druga) {
	return prva._drzava == druga._drzava && prva._vazenjeOD == druga._vazenjeOD && prva._vazenjeDO == druga._vazenjeDO && prva._napomena == druga._napomena;
}
class PoslovnaViza :public Viza {
	string _kompanija; //naziv kompanije koja potvrðuje da posluje sa aplikantom
public:
	//PoslovnaViza poslovna(SAD, dat15022017, dat18082017, "Microsoft");
	PoslovnaViza(Drzava drzava, Datum _OD, Datum _DO, string kompanija, char * napomena = nullptr) : Viza(drzava, _OD, _DO, napomena) {
		_kompanija = kompanija;
	}
	~PoslovnaViza() {};
	PoslovnaViza(const PoslovnaViza &obj) : Viza(obj) {
		_kompanija = obj._kompanija;
	}
	PoslovnaViza & operator = (const PoslovnaViza &obj) {
		if (this != &obj) {
			delete[]_napomena;
			if (obj._napomena != nullptr) {
				_napomena = new char[strlen(obj._napomena) + 1];
				strcpy_s(_napomena, strlen(obj._napomena) + 1, obj._napomena);
			}
			else
				_napomena = nullptr;
			_vazenjeDO = obj._vazenjeDO;
			_vazenjeOD = obj._vazenjeOD;
			_drzava = obj._drzava;
			_kompanija = obj._kompanija;
		} return *this;
	}
	virtual void Info() {
		cout << "Poslovna Viza" << endl;
	}
	friend ostream & operator << (ostream & COUT, PoslovnaViza & obj);
};
ostream & operator << (ostream & COUT, PoslovnaViza & obj) {
	COUT << _ispisDrzave[obj._drzava] << " " << obj._vazenjeOD << " " << obj._vazenjeDO << " " << obj._kompanija << endl;
	COUT << ((obj._napomena != nullptr) ? obj._napomena : "") << endl;
	COUT << "STATUSI -> " << obj._statusi << endl;
	return COUT;
}

class TuristickaViza : public Viza {
	string _adresa; //adresa hotele ili drugog smještaja na kojoj æe aplikant boraviti
public:
	//TuristickaViza turisticka(ENGLESKA, dat18082017, dat11122017, "Whitehall Pl, London SW1A 2BD, UK");
	TuristickaViza(Drzava drzava, Datum _OD, Datum _DO, string adresa, char * napomena = nullptr) : Viza(drzava, _OD, _DO, napomena) {
		_adresa = adresa;
	}
	~TuristickaViza() {};
	TuristickaViza(const TuristickaViza &obj) : Viza(obj) {
		_adresa = obj._adresa;
	}
	TuristickaViza & operator = (const TuristickaViza &obj) {
		if (this != &obj) {
			delete[]_napomena;
			if (obj._napomena != nullptr) {
				_napomena = new char[strlen(obj._napomena) + 1];
				strcpy_s(_napomena, strlen(obj._napomena) + 1, obj._napomena);
			}
			else
				_napomena = nullptr;
			_vazenjeDO = obj._vazenjeDO;
			_vazenjeOD = obj._vazenjeOD;
			_drzava = obj._drzava;
			_adresa = obj._adresa;
		} return *this;
	}
	virtual void Info() {
		cout << "Turisticka Viza" << endl;
	}
	friend ostream & operator << (ostream & COUT, TuristickaViza & obj);
};
ostream & operator << (ostream & COUT, TuristickaViza & obj) {
	COUT << _ispisDrzave[obj._drzava] << " " << obj._vazenjeOD << " " << obj._vazenjeDO << " " << obj._adresa << endl;
	COUT << ((obj._napomena != nullptr) ? obj._napomena : "") << endl;
	COUT << "STATUSI -> " << obj._statusi << endl;
	return COUT;
}
void Print(Viza *viza) {
	strazar.lock();
	cout << *viza << endl;
	this_thread::sleep_for(chrono::seconds(1));
	strazar.unlock();
}
class Aplikant {
	char * _imePrezime;
	Datum * _datumRodjenja;
	char * _brojPasosa;
	vector<Viza *> _vize;
public:
	//Aplikant jasmin("Jasmin Azemovic", Datum(15, 5, 1979), "BS6985645");
	Aplikant(char * imePrezime, Datum datumRodjenja, char * brPasosa) {
		int vel = strlen(imePrezime) + 1;
		_imePrezime = new char[vel];
		strcpy_s(_imePrezime, vel, imePrezime);
		vel = strlen(brPasosa) + 1;
		_brojPasosa = new char[vel];
		strcpy_s(_brojPasosa, vel, brPasosa);
		_datumRodjenja = new Datum(datumRodjenja);
	}
	~Aplikant() {
		delete[]_imePrezime; _imePrezime = nullptr;
		delete[]_brojPasosa; _brojPasosa = nullptr;
		delete _datumRodjenja; _datumRodjenja = nullptr;
		for (size_t i = 0; i < _vize.size(); i++)
		{
			delete _vize[i]; _vize[i] = nullptr;
		}
	}
	Aplikant(const Aplikant &obj) {
		int vel = strlen(obj._imePrezime) + 1;
		_imePrezime = new char[vel];
		strcpy_s(_imePrezime, vel, obj._imePrezime);
		vel = strlen(obj._brojPasosa) + 1;
		_brojPasosa = new char[vel];
		strcpy_s(_brojPasosa, vel, obj._brojPasosa);
		_datumRodjenja = new Datum(*obj._datumRodjenja);
		_vize = obj._vize;
	}
	Aplikant & operator = (Aplikant &obj) {
		if (this != &obj) {
			delete[]_imePrezime;
			delete[]_brojPasosa;
			delete _datumRodjenja;
			for (size_t i = 0; i < _vize.size(); i++)
			{
				delete _vize[i]; _vize[i] = nullptr;
			}
			int vel = strlen(obj._imePrezime) + 1;
			_imePrezime = new char[vel];
			strcpy_s(_imePrezime, vel, obj._imePrezime);
			vel = strlen(obj._brojPasosa) + 1;
			_brojPasosa = new char[vel];
			strcpy_s(_brojPasosa, vel, obj._brojPasosa);
			_datumRodjenja = new Datum(*obj._datumRodjenja);
			_vize = obj._vize;
		} return *this;
	}

	/*
	//DodajVizu(&poslovna)
	NOVA VIZA SE NE MOŽE DODATI UKOLIKO:
	-	JE APLIKANT MLAÐI OD 10 GODINA
	-	U TOM PERIODU (ODNOSI SE NA PERIOD VIZE OD - DO) NE POSTOJI VAŽEÆA (POJAM VAŽEÆA PODRAZUMIJEVA DA JE POSLJEDNJI STATUS VIZE BIO IZDATA TJ. DA VIZA NIJE PONIŠTENA ILI ISTEKLA) VIZA ZA TU DRŽAVU, BEZ OBZIRA NA NJENU VRSTU.
	-	JE NEKA OD RANIJIH VIZA PONIŠTENA (OVO SE ODNOSI SAMO NA VIZE ZA DRŽAVU: SAD)
	//char * _imePrezime;
	//Datum * _datumRodjenja;
	//char * _brojPasosa;
	//vector<Viza *> _vize;
	*/
	bool DodajVizu(Viza *viza) {
		if (_datumRodjenja->GodineStarosti() < 10)
			return false;

		for (int i = 0; i < _vize.size(); i++) {
			for (int j = 0; j < _vize[i]->GetStatuse().GetTrenutno(); j++) {
				if (*_vize[i]->GetStatuse().GetT1()[j] == PONISTENA && _vize[i]->GetDrzava() == SAD)
					return false;
				if (viza->GetDrzava() == _vize[i]->GetDrzava() && ((viza->GetVazenjeOD() >= _vize[i]->GetVazenjeOD() && _vize[i]->GetVazenjeDO() >= viza->GetVazenjeOD()) || (viza->GetVazenjeDO() <= _vize[i]->GetVazenjeDO() && viza->GetVazenjeDO() >= _vize[i]->GetVazenjeOD()) && (*_vize[i]->GetStatuse().GetT1()[j] == IZDATA || *_vize[i]->GetStatuse().GetT1()[j] == PONISTENA || *_vize[i]->GetStatuse().GetT1()[j] == ISTEKLA)))
					return false;
			}
		}

		PoslovnaViza *pokPoslovna = dynamic_cast<PoslovnaViza*>(viza);
		if (pokPoslovna != nullptr)
			_vize.push_back(new PoslovnaViza(*pokPoslovna));
		else {
			TuristickaViza *pokTuristicka = dynamic_cast<TuristickaViza*>(viza);
			if (pokTuristicka != nullptr)
				_vize.push_back(new TuristickaViza(*pokTuristicka));
		}
		return true;
	}
	/*
	//DodajStatus(&poslovna, PONISTENA, "predstavnicima kompanije Microsoft nemaju informaciju o posjeti navedenog aplikanta")
	STATUSI SE MOGU DODAVATI SAMO U REDOSLIJEDU U KOJEM SE NALAZE U ENUMERACIJI NPR.
	-	NIJE MOGUÆE ODBITI VIZU AKO PRETHODNO NIJE IMALA STATUS DA JE ZAHTJEV_ZAPRIMLJEN;
	-	NIJE MOGUÆE PONIŠTITI VIZU KOJA PRETHODNO NIJE IZDATA
	//char * _imePrezime;
	//Datum * _datumRodjenja;
	//char * _brojPasosa;
	//vector<Viza *> _vize;
	*/
	bool DodajStatus(Viza *viza, Status status, char * komentar = "") {
		for (int i = 0; i < _vize.size(); i++) {
			if (*_vize[i] == *viza) {
				bool izdata = false, zahtjev = false;
				for (int j = 0; j < _vize[i]->GetStatuse().GetTrenutno(); j++) {
					if (*_vize[i]->GetStatuse().GetT1()[j] == ZAHTJEV_ZAPRIMLJEN)
						zahtjev = true;
					if (*_vize[i]->GetStatuse().GetT1()[j] == IZDATA)
						izdata = true;
				}

				if ((!izdata && status == PONISTENA) || (!zahtjev && status == ODBIJENA))
					return false;

				_vize[i]->GetStatuse().Dodaj(status, komentar);
				return true;
			}
		}
		return false;
	}
	//U ZASEBNOJ NITI URADITI PRINTANJE VIZE...KORISTITI METODU SLEEP_FOR
	//PrintajVizu(&turisticka)
	void PrintajVizu(Viza *viza) {
		thread printThread(Print, ref(viza));
		printThread.join();
	}
	//VRAÆA PODATKE O SVIM VIZAMA KOJE SU IZDATE ZA ODREÐENU DRŽAVU, A KOJE SU IMALE ODREÐENI STATUS
	//vector<Viza *> jasminVize = jasmin.GetVizeByDrzava(SAD, PONISTENA);
	vector<Viza *> GetVizeByDrzava(Drzava drzava, Status status) {
		vector<Viza *> _lista;
		for (int i = 0; i < _vize.size(); i++) {
			if (_vize[i]->GetDrzava() == drzava) {
				for (int j = 0; j < _vize[i]->GetStatuse().GetTrenutno(); j++) {
					if (*_vize[i]->GetStatuse().GetT1()[j] == status) {
						PoslovnaViza *pokPoslovna = dynamic_cast<PoslovnaViza*>(_vize[i]);
						if (pokPoslovna != nullptr)
							_lista.push_back(new PoslovnaViza(*pokPoslovna));
						else {
							TuristickaViza *pokTuristicka = dynamic_cast<TuristickaViza*>(_vize[i]);
							if (pokTuristicka != nullptr)
								_lista.push_back(new TuristickaViza(*pokTuristicka));
						}
					}
				}
			}
		}
		return _lista;
	}
	//UNUTAR NAPOMENE MIJENJA SADRŽAJ "nemaju informaciju" SA "su upoznti sa" - KORISTITI REGEX
	//jasmin.ZamijeniRijec("nemaju informaciju", "su upoznti sa");
	void ZamijeniRijec(string rijec, string zamjena) {
		regex reg("\\s");
		rijec = regex_replace(rijec, reg, "\\s");
		for (int i = 0; i < _vize.size(); i++) {
			for (int j = 0; j < _vize[i]->GetStatuse().GetTrenutno(); j++) {
				regex rege(rijec);
				string status = *_vize[i]->GetStatuse().GetT2()[j];
				status = regex_replace(status, rege, zamjena);
				cout << status << endl;
			}
		}
	}
	friend ostream & operator << (ostream & COUT, Aplikant & obj);
	vector<Viza * > GetVize() { return _vize; }
};
ostream & operator << (ostream & COUT, Aplikant & obj) {
	COUT << obj._imePrezime << " " << *obj._datumRodjenja << " " << obj._brojPasosa << endl;
	COUT << "VIZE -> " << endl;
	for (size_t i = 0; i < obj._vize.size(); i++)
		COUT << *obj._vize[i] << endl;
	return COUT;
}
ostream & operator << (ostream &COUT, Viza &viza) {
	PoslovnaViza *pokP = dynamic_cast<PoslovnaViza*>(&viza);
	if (pokP != nullptr) {
		COUT << *pokP;
	}
	else {
		TuristickaViza *pokT = dynamic_cast<TuristickaViza*>(&viza);
		if (pokT != nullptr) {
			COUT << *pokT;
		}
	}
	return COUT;
}
void main() {

	/****************************************************************************
	1. SVE KLASE TREBAJU POSJEDOVATI ADEKVATAN DESTRUKTOR
	2. NAMJERNO IZOSTAVLJANJE KOMPLETNIH I/ILI POJEDINIH DIJELOVA DESTRUKTORA KOJI UZROKUJU RUNTIME ERROR ÆE BITI OZNACENO KAO "RE"
	3. SPAŠAVAJTE PROJEKAT KAKO BI SE SPRIJEÈILO GUBLJENJE URAÐENOG ZADATKA
	4. PROGRAMSKI CODE SE TAKOÐER NALAZI U FAJLU CODE.TXT
	5. NAZIVI FUNKCIJA, TE BROJ I TIP PARAMETARA MORAJU BITI IDENTIÈNI ONIMA KOJI SU KORIŠTENI U TESTNOM CODE-U, OSIM U SLUÈAJU DA POSTOJI ADEKVATAN RAZLOG ZA NJIHOVU MODIFIKACIJU. OSTALE, POMOÆNE FUNKCIJE MOŽETE IMENOVATI I DODAVATI PO ŽELJI.
	6. IZUZETAK BACITE U FUNKCIJAMA U KOJIMA JE TO NAZNAÈENO.
	****************************************************************************/

	Datum danas(15, 2, 2017);
	Datum sutra(++danas); // inkrementiranjem se uvecava dan za 1
	Datum prekosutra;
	Datum dat15022017(15, 2, 2017), dat18082017(18, 8, 2017), dat11052018(11, 5, 2018), dat11122017(11, 12, 2017);

	prekosutra = ++danas;
	if (prekosutra == danas)
		cout << "Datumi su jednaki" << endl;
	cout << danas << endl << sutra << endl << prekosutra << crt;

	int v6 = 6, v13 = 13, v32 = 32, v63 = 63, v98 = 98, v109 = 109, v196 = 196;

	FITKolekcija<int, int, 7> brojevi;
	brojevi.Dodaj(v196, v6);
	brojevi.Dodaj(v13, v32);
	brojevi.Dodaj(v98, v196);
	brojevi.Dodaj(v63, v13);
	brojevi.Dodaj(v98, v196);
	brojevi.Dodaj(v196, v6);
	brojevi.Dodaj(v63, v32);

	cout << brojevi << crt;
	//NA KOJI NAÈIN ÆE SE VRŠITI SORTIRANJE PRVENSTVENO ZAVISI OD VRIJEDNOSTI PARAMETRA, A VRŠI SE NA OSNOVU VRIJEDNOSTI TIPA T1
	brojevi.Sortiraj(RASTUCI);
	cout << brojevi << crt;
	brojevi.Sortiraj(OPADAJUCI);
	cout << brojevi << crt;

	try
	{
		//U SLUÈAJU PREKORAÈENJA DEFINISANE VELIÈINE KOLEKCIJE, POTREBNO JE BACITI IZUZETAK
		brojevi.Dodaj(v13, v196);
	}
	catch (exception & err)
	{
		cout << "Error -> " << err.what() << endl;
	}

	Aplikant jasmin("Jasmin Azemovic", Datum(15, 5, 1979), "BS6985645");
	PoslovnaViza poslovna1(SAD, dat15022017, dat18082017, "Microsoft");
	PoslovnaViza poslovna2(SAD, dat18082017, dat11052018, "Vaynermedia");
	PoslovnaViza poslovna3(AUSTRALIJA, dat11122017, dat11052018, "Apple");
	TuristickaViza turisticka1(ENGLESKA, dat18082017, dat11122017, "Whitehall Pl, London SW1A 2BD, UK");
	TuristickaViza turisticka2(ENGLESKA, dat15022017, dat11122017, "Adresa, UK");
	TuristickaViza turisticka3(SAD, dat11122017, dat11052018, "Adresa, SAD");

	if (jasmin.DodajVizu(&poslovna1))
		cout << "Podaci o vizi uspješno zaprimljeni" << endl;
	if (jasmin.DodajVizu(&poslovna2))
		cout << "Podaci o vizi uspješno zaprimljeni" << endl;
	if (jasmin.DodajVizu(&poslovna3))
		cout << "Podaci o vizi uspješno zaprimljeni" << endl;
	if (jasmin.DodajVizu(&turisticka1))
		cout << "Podaci o vizi uspješno zaprimljeni" << endl;
	if (jasmin.DodajVizu(&turisticka2))
		cout << "Podaci o vizi uspješno zaprimljeni" << endl;
	if (jasmin.DodajVizu(&turisticka3))
		cout << "Podaci o vizi uspješno zaprimljeni" << endl;

	if (jasmin.DodajStatus(&poslovna1, ZAHTJEV_ZAPRIMLJEN, "provjeriti navode sa predstavnicima kompanije Microsoft"))
		cout << "Status vize uspijesno promijenjen" << endl;
	else
		cout << "Aplikant " << jasmin << " ne posjeduje evidentiranu trazenu vizu ili status vize nema odgovarajucu vrijednost" << endl;

	if (jasmin.DodajStatus(&poslovna1, PONISTENA, "predstavnicima kompanije Microsoft nemaju informaciju o posjeti navedenog aplikanta"))
		cout << "Status vize uspijesno promijenjen" << endl;
	else
		cout << "Aplikant " << jasmin << " ne posjeduje evidentiranu trazenu vizu ili status vize nema odgovarajucu vrijednost" << endl;

	if (jasmin.DodajStatus(&poslovna1, ODBIJENA, "predstavnicima kompanije Microsoft nemaju informaciju o posjeti navedenog aplikanta"))
		cout << "Status vize uspijesno promijenjen" << endl;
	else
		cout << "Aplikant " << jasmin << " ne posjeduje evidentiranu trazenu vizu ili status vize nema odgovarajucu vrijednost" << endl;

	if (jasmin.DodajStatus(&turisticka1, ZAHTJEV_ZAPRIMLJEN))
		cout << "Status vize uspijesno promijenjen" << endl;
	else
		cout << "Aplikant " << jasmin << " ne posjeduje evidentiranu trazenu vizu ili status vize nema odgovarajucu vrijednost" << endl;

	if (jasmin.DodajStatus(&turisticka1, IZDATA)) {
		cout << "Status vize uspijesno promijenjen" << endl;
		//U ZASEBNOJ NITI URADITI PRINTANJE VIZE...KORISTITI METODU SLEEP_FOR
		jasmin.PrintajVizu(&turisticka1);
	}
	else
		cout << "Aplikant " << jasmin << " ne posjeduje evidentiranu trazenu vizu ili status vize nema odgovarajucu vrijednost" << endl;
	//VRAÆA PODATKE O SVIM VIZAMA KOJE SU IZDATE ZA ODREÐENU DRŽAVU, A KOJE SU IMALE ODREÐENI STATUS
	vector<Viza *> jasminVize = jasmin.GetVizeByDrzava(SAD, ZAHTJEV_ZAPRIMLJEN);

	//UNUTAR NAPOMENE MIJENJA SADRŽAJ "nemaju informaciju" SA "su upoznti sa" - KORISTITI REGEX
	jasmin.ZamijeniRijec("nemaju informaciju", "su upoznti sa"); //Ne postoji slicna napomena, pa sam radio na statusima (gdje postoji slican tekst...)
	system("pause>0");
}