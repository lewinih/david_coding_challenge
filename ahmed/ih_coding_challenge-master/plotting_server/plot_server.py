from flask import Flask, request, jsonify, make_response, Response
import matplotlib.pyplot as plt
import json
import os

app = Flask("name", static_folder='static')

@app.route('/receive_data', methods=['GET','POST', 'DELETE'])
def receive_data():
    # checking the request has the correct content type
    if request.headers.get('Content-Type') != 'application/json':
        return jsonify({'error': 'Invalid content type'}), 400

    data = request.json

    # Validation
    if 'x_data' not in data or 'intensity_data' not in data:
        return jsonify({'error': 'Missing data'}), 400

    title = data.get('title', 'MyDataPlot')
    x_data = data['x_data']
    intensity_data = data['intensity_data']

    # Plotting 
    plt.figure(figsize=(10, 5))
    plt.plot(x_data, intensity_data, label="Intensity Data")
    plt.xlabel("Time (x)")
    plt.ylabel("Intensity")
    plt.title(title)
    plt.legend()
    plt.grid(True)

    # Save the plot
    plot_filename = f"{title}.png"
    plt.savefig(plot_filename)
    

    # Generate response
    response_data = {
        'message': 'Data received and plotted.',
        'plot_filename': plot_filename,
        'plot_url': request.url_root.strip('/') + '/' + plot_filename  
    }

    return jsonify(response_data), 200
    

if __name__ == '__main__':
    app.run(debug=True)
