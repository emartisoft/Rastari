# ~/.profile: executed by Bourne-compatible login shells.

rastariCheck=`ps -x | grep "run\.sh"`

if test -z "$rastariCheck"
then
  rastariHome=/usr/local/rastari
  version=$(<$rastariHome/VERSION)
  clear
  # set font for terminal
  setfont $rastariHome/font/atarist-normal.psf.gz
  # set background for terminal
  setterm -background black -foreground green --bold on --store
  # Rastari Configuration is run on first booting into Rastari
  $rastariHome/Configuration.sh
  export PS1="\W \$"
  clear
  # write ascii picture for "Rastari" text
  figlet -k Rastari
  echo "Rastari V${version} by emarti, Murat Ozdemir"
  uname -a
  if [ "$BASH" ]; then
    if [ -f ~/.bashrc ]; then
      . ~/.bashrc
    fi
  fi

  mesg n

  # run rastari
  cd $rastariHome
  ./run.sh
else
  # Rastari already running
  if [ "$BASH" ]; then
    if [ -f ~/.bashrc ]; then
      . ~/.bashrc
    fi
  fi
fi
