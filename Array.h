#ifndef __Array__
#define __Array__

template <class Element>
class Array
{
  Element** array;
  int max;
  int nb;
  int no;
  
public:
  Array( int n )
  { 
    max = n; 
    nb = no = 0;

    array = (Element**) malloc( max * sizeof(Element) );
  };

//  void operator() ( int no, Element* element ) { list[ no ] = element; };
  void operator+=( Element* element ) { add(element); };
  void add( Element* element )
  {
    if ( nb < max )
      array[ nb++ ] = element;
    else
    {
      Serial.println("Error : Increase arry max constant.");
      //throw 
    };
  };


  int size(void) { return nb; };
  int iter(void) { return no; };
  Element* first(void) { no = 0; return next(); };
  Element* operator[]( int n ) { no = n; return next(); };
  Element* next(void)  { if (no < nb) return array[ no++ ]; return NULL; };
};

#endif
