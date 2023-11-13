import matplotlib.pyplot as plt
import numpy  as np

X, Y = np.loadtxt('Test.txt', delimiter=',', unpack=True)
X1, Y1 = np.loadtxt('Filter.txt', delimiter=',', unpack=True)
X2, Y2 = np.loadtxt('peak.txt', delimiter=',', unpack=True)

# Create a plot  
plt.figure(figsize=(10, 5))
plt.xlabel("Position (x)")
plt.ylabel("Intensity")
plt.title("ip_Graph Plot")
plt.plot(Y,X, label = "Input Intensity" ) 
plt.plot(X1,Y1, label = "Filter Intensity" ) 
plt.plot(Y2,X2, label = "Peak Intensity" )  
plt.legend()
plt.grid(True)
plt.savefig('plot.png')  
plt.show() 
plt.close()



'''from flask import Flask, request, jsonify
import matplotlib.pyplot as plt

app = Flask("name")

@app.route('/receive_data', methods=['POST'])
def receive_data():
    data = request.json  # Expecting JSON data with 'x_data' and 'intensity_data'
    try:
        title = data['title']
    except KeyError:
        title = 'sample_plot'
    x_data = data['x_data']
    intensity_data = data['intensity_data']

    # Create a time series plot
    plt.figure(figsize=(10, 5))
    plt.plot(x_data, intensity_data, label="Intensity Data")
    plt.xlabel("Time (x)")
    plt.ylabel("Intensity")
    plt.title("Time Series Plot")
    plt.legend()
    plt.grid(True)

    plt.savefig(f"{title}.png")
    return 'Data received and plotted.'

if __name__ == '__main__':
    app.run(debug=True) '''
