%define name wmcoincoin
%define version 2.6.0
%define release 1
Summary:	Funny dock-app for browsing and interact to XML board sites    
Summary(fr): Client pour naviguer et interagir sur des tribunes web en XML
Name:		%{name}
Version:	%{version}
Release:	%{release}
Source:		%{name}-%{version}.tar.gz
Source1:    http://hules.free.fr/wmcoincoin/download/wmcoincoin-2.5.1f.tar.gz 
License:	GPLv2 
URL:		http://hules.free.fr/wmcoincoin/wmcoincoin.html
Group:		Applications/Communications
BuildRoot:	%{_tmppath}/%{name}-buildroot
Prefix:		%{_prefix}
BuildRequires:  gtk+-devel imlib2-devel
  

%description
Program designed to consult and post to XML web-chats without web browser
%description -l fr
Programme conçu pour poster sur des chats XML sans navigateur web 

%prep
rm -rf $RPM_BUILD_ROOT
%setup 

%build
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
%find_lang %{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files -f %{name}.lang
%defattr(-,root,root,0755)
%doc README NEWS COPYING AUTHORS 
%{_bindir}/*
%{_datadir}/%{name}
%{_datadir}/%{name}/options
%{_datadir}/%{name}/*.xpm
%{_datadir}/wmcoincoin

%changelog

* Wed Jun 6  2012 pierre80 <pierre80@voila.fr> 2.5.1f-1
- Version 2.5.1f-1
- clean and update spec for compatibility with fedora packages guidlines

* Tue Jul 19 2003 houpla <laurent@pschit.net> 2.4.6a-1
- version 2.4.6a-1
- clean and update spec

* Sat Jun  1 2002 pouaite <c0in@altern.org> 2.3.8a-1
- version 2.3.8a

* Fri Apr 26 2002 pouaite <c0in@altern.org> 2.3.7b-1
- version 2.3.7b

* Sun Apr 14 2002 pouaite <c0in@altern.org> 2.3.7a-1
- version 2.3.7a

* Sat Mar  9 2002 pouaite <c0in@altern.org> 2.3.6-1
- version 2.3.6

* Mon Feb 18 2002 pouaite <c0in@altern.org> 2.3.5-1
- version 2.3.5

* Sun Jan 20 2002 pouaite <c0in@altern.org> 2.3.4-1
- version 2.3.4

* Sun Dec 16 2001 pommier julien <c0in@altern.org> 2.3.2-1
- version 2.3.2

* Sun Nov 18 2001 pommier julien <c0in@altern.org> 2.2.4beta-1
- update to v2.2.4 after  modifications of dacode

* Sun Nov 18 2001 pommier julien <c0in@altern.org> 2.2.3beta-2
- renomage de remote.php3 en remote.rdf, gestion des tags '<br />'

* Mon Nov 12 2001 pommier julien <c0in@altern.org> 2.2.3-1
- gestion de la messagerie, améliorations diverses

* Tue Oct 23 2001 pommier julien <c0in@altern.org> 2.2beta-1 - réecriture de l'affichage des
news, coup de pied dans le cul de la libwraster, GROSSE modif de la boucle principal (pour éviter
des plantages rares mais bizarres). Ajout d'un TrollDetector(tm)(c)MAPL

* Thu Oct 11 2001 pommier julien <c0in@altern.org> 2.11beta-1
- merge de la branche cygwin de gege, et ajout de petits bugfixes

* Sun Sep 30 2001 pommier julien <c0in@altern.org> 2.1beta-1
- après l'ignoble v2.0, voici venir la v2.1beta et son cortège de fonctionnalités.

* Tue Sep 25 2001 pommier julien <c0in@altern.org> 2.0-2 
- correction d'un bug mineur (certains messages dans le remote.php3
  sont répartis sur plus d'une ligne..)
* Sun Sep 23 2001 pommier julien <c0in@altern.org> 2.0-1
- tentative de création du rpm...


# end of file
