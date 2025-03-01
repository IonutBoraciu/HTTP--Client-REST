Boraciu Ionut-Sorin 325CA

Am pornit de la rezolvarea mea a laboratorului 9.

	Comenzile pe care programul le execute sunt : 
		-  register : creeaza un json care contine user-ul si parola care
		urmeaza a fi trimise catre server cu un request de tip post pentru a vedea
		daca user-ul este liber si putem crea un nou cont, sau este ocupat. In functie
		de acest lucru sunt afisate mesaje corespunzatoare.

		-  login : asteapta de la utilizator un user si o parola, si trimite o cerere
		de tip get catre server, daca exista utilizatorul si parola este corecta,server
		va returna un cookie pentru a putea demonstra ca suntem conectati la server cu
		user-ul dat.

		-  enter_library : folosind cookie-ul care atesta sesiune create cu server-ul
		incercam sa obtinem un token cu ajutorul caruia vom putea accesa libraria de
		carti aferenta utilizatorului. Daca nu suntem conectati cu un user vom afisa un
		mesaj de eroare.
		
		-  get_books : daca user-ul este conectat si am obtinut token-ul pentru accesul
		la librarie putem trimite catre server un request de tip get pentru a putea
		vizualiza toate cartile din libraria utilizatorului curent.
		
		-  add_book : utilizatorului i se va oferi un promp in care trebuia sa scrie 
		toate datele necesare unei carti ( ne asiguram ca nu a dat fail niciuna dintre
		citiri).
		
		-  get_book : incercam sa facem o cerere get catre server pentru cartea cu un
		id introdus de utilizator, daca acest cartea cu id-ul dat nu exista se va afisa
		un mesaj de eroare.
		
		-  detele_book : functioneaza in acelasi fel ca si get_book, singura diferenta
		fiind ca se face o cerere de tip DELETE, in loc sa scriem GET vom scrie DELETE.
		Daca cartea pe care vrem sa o stergem nu exista se afiseaza un mesaj aferent.
		
		-  logout : user-ul se deconecteaza de la server si sunt golite cookie-urile si
		access_token-ul aferente acestuia
		
		-  exit : se opreste executia programului ( si se elibereaza memoria alocata
		dinamic)
	Am ales sa folosesc libraria parson.h de pe github-ul oferit in cerinta temei.
	Pentru a crea un json m-am folosit de structurile JSON_File si JSON_Value. O functie
	folisitoare din librarie este: json_serialize_to_string_pretty, pe care am folosit-o
	pentru a afisa pe ecran informatiile intr-un sir de caractere frumos.
