# import gmplot package
import gmplot


def Bring_Data(filename):
    with open (filename, 'r') as f:
        return f.read()


def Get_Long_Lat(data):
    temp = data.split('L')
    long_lat_list = []
    for item in temp:
        long_lat_list.append(item.split('G'))
    long_lat_list = long_lat_list[1:]
    return long_lat_list

def split_Into_twoLists(long_lat_list):
    longtitude_list = []
    latitude_list = []
    for coordinate in long_lat_list:
        latitude_list.append(float(coordinate[0]))
        longtitude_list.append(float(coordinate[1]))
    return longtitude_list, latitude_list


def main():
    data = Bring_Data("hazem.txt")
    long_lat_list = Get_Long_Lat(data)
    longitude_list, latitude_list = split_Into_twoLists(long_lat_list)
    print(longitude_list)
    print(latitude_list)
    # latitude_list = [ 30.3358376, 30.307977, 30.3216419 ]
    # longitude_list = [ 77.8701919, 78.048457, 78.0413095 ]
    # longitude_list = [ 31.278704, 31.278704 ]
    # latitude_list=[30.307977, 30.307977]
    gmap3 = gmplot.GoogleMapPlotter(30.064792,
                                31.278698, 40)
  
    # scatter method of map object 
    # scatter points on the google map
    gmap3.scatter( latitude_list, longitude_list, '#FF0000',
                                size = 0.01, marker = False )
    
    # Plot method Draw a line i13n
    # between given coordinates
    gmap3.plot(latitude_list, longitude_list, 
            'cornflowerblue', edge_width = 2.5)
    
    gmap3.draw( "map13.html" )




  

main()
