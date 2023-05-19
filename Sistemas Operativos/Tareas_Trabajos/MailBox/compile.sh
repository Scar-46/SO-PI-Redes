make clean
make DEFS=-DSND
mv bin/Sistemas ./MailSND
make clean
make DEFS=-DRCV
mv bin/Sistemas ./MailRCV