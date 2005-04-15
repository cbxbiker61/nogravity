Summary: No Gravity is a fantastic and futuristic universe made of five intergalactic worlds.
Name: nogravity
Version: 1.98r9
Release: 1
License: GPL
Group: Amusements/Games
Vendor: realtech VR
Packager: Matt Williams <maffy@sourceforge.net>
URL: http://www.realtech-vr.com/nogravity/
Source: http://prdownloads.sourceforge.net/nogravity/rt-nogravity-src-198.zip?use_mirror=ovh
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description

No Gravity is a fantastic and futuristic universe made of five intergalactic worlds. An arcade type game with great playability, where it is easy to plunge into space battles against spacefighters, space stations and more!

No Gravity is a space shooter in 3D 'a la Wing Commander'. The player is controlling a space ship view from the first person. In each mission, the player need to accomplish some objectives, like shoot and destroys enemy ships or base, escorting some ships, clearing mine field, etc....

A brief description of the objectives is given at the beginning of a mission. In some situations, the player has some wingmen, and it can controls them by giving order like attack enemy, retreat, protect the base etc... A mission is composed of different sectors which has differents objectives.

When the objectives of the current sector is complete, the player needs to go the nearest 'NAV' point, when he can 'wrap' to the next sector. In the last sector, a 'warp' gate is apparearing and the player need to go through it and complete the level. At the end of each episodes (one episode is 10 missions), there is a large base station to destroy. When the player completes all the episodes (5 episodes), the game ends. The mission fails if : - The player got destroyed or - if an objective is not complete (like an important ship being destroyed by enemy).

To play, you will also need to download the game data from http://www.realtech-vr.com/nogravity/.

%prep
cd $RPM_BUILD_DIR
mkdir -p $RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
cd $RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
unzip -o $RPM_SOURCE_DIR/rt-nogravity-src-198.zip
if [ $? -ne 0 ]; then
  exit $?
fi
chown -R root.root .
chmod -R a+rX,g-w,o-w .

%build
cd $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION/src/Linux
./bootstrap
./configure --enable-sound=sdl_mixer
make

%install
cd $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION/src/Linux
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/local/games/nogravity/nogravity
/usr/local/games/nogravity/GNU.TXT
/usr/local/games/nogravity/README.TXT

%changelog
* Tue Mar 22 2005 Matt Williams <maffy@sourceforge.net> - 
- Linux/SDL port.
