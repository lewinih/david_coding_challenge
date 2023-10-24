from flask import Flask, request, jsonify
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
    app.run(debug=True)
