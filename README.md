# DBproject2

--------------------------
Project Info
--------------------------

Database System Implementation (COMS 4112)  
Professor Kenneth A. Ross  
Columbia University  
Spring 2016  

    Uzo Amuzie    -   Co-Project Lead
    Tommy Orok    -   Co-Project Lead

Project specs located in "project2.pdf"


--------------------------
Running the Program
--------------------------

$ make  
$ ./build \<K\> \<P\> \<int\> ...

It's that easy!


--------------------------
Things to Note
--------------------------

1) We noticed that every other probe generated (i.e 2nd, 4th, 6th, etc.) is already present in the list of random keys generated. We are unsure of the root cause of this phenomenon

2) We did not have a chance to implement the corner case of preventing a new node from being allocated in memory when there are not enough keys to start node. For example, in our version, it is mpossible for us to have a node full of MAXINT