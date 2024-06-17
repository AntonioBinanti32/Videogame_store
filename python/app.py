from flask import Flask, render_template, request, redirect, url_for, session, flash
import requests
import configparser
from datetime import datetime

app = Flask(__name__)
app.secret_key = 'your_secret_key'

config = configparser.ConfigParser()
config.read('config.ini')

backend_url = config.get('backend', 'backend_url')


def is_user_logged_in():
    return 'user' in session

def getHeaders():
    user = session.get('user')
    token = session.get('token')

    headers = {
        'Token': token,
        'ActualUser': user
    }

    return headers

def getUserInformation():
    user = session.get('user')
    response_user = requests.get(f'{backend_url}/getUser/{user}', headers=getHeaders())
    return response_user.json()

# Definisci il filtro personalizzato per la formattazione della data
@app.template_filter('dateformat')
def dateformat(value):
    try:
        if isinstance(value, (int, float)):
            # Se il valore è un timestamp Unix, convertilo in datetime
            dt_object = datetime.utcfromtimestamp(value / 1000.0)
        elif isinstance(value, dict) and '$date' in value:
            # Se il valore è un oggetto data MongoDB
            dt_object = datetime.utcfromtimestamp(value['$date'] / 1000.0)
        else:
            raise ValueError("Formato data non riconosciuto")

        # Formatta la data come desiderato (es. "%Y-%m-%d %H:%M:%S")
        formatted_date = dt_object.strftime("%Y-%m-%d %H:%M:%S")
        return formatted_date
    except Exception as e:
        print(f"Errore nella formattazione del timestamp: {e}")
        return "Data non disponibile"



@app.route('/')
def home():
    try:
        if not is_user_logged_in():
            return redirect(url_for('login'))

        actualUser = getUserInformation()

        response_games = requests.get(f'{backend_url}/getGames', headers = getHeaders())
        games = response_games.json()
        return render_template('home.html', games=games, actualUser=actualUser)
    except requests.exceptions.HTTPError as http_err:
        flash('Login failed: invalid credentials')
    except requests.exceptions.ConnectionError as conn_err:
        flash('Connection error: please try again later')
    except requests.exceptions.Timeout as timeout_err:
        flash('Request timed out: please try again later')
    except requests.exceptions.RequestException as req_err:
        flash('An unexpected error occurred: please try again later')
    return redirect(url_for('login'))

@app.route('/game/<string:gameTitle>')
def game(gameTitle):
    response = requests.get(f'{backend_url}/getGameByTitle/{gameTitle}', headers = getHeaders())
    game = response.json()
    return render_template('game.html', game=game)


@app.route('/cart')
def cart():
    if 'cart' not in session:
        session['cart'] = []
    cart_items = session['cart']
    return render_template('cart.html', cart_items=cart_items)


@app.route('/add_to_cart/<string:gameTitle>')
def add_to_cart(gameTitle):
    if 'cart' not in session:
        session['cart'] = []
    session['cart'].append(gameTitle)
    return redirect(url_for('cart'))


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        credentials = request.form
        try:
            response = requests.post(f'{backend_url}/login', json=credentials)
            response.raise_for_status()  # Questo genera un'eccezione per codici di stato HTTP 4xx/5xx
            session['user'] = response.json()['user']
            session['token'] = response.json()['token']
            user = getUserInformation()
            session['user_image'] = user['image_url']
            return redirect(url_for('home'))
        except requests.exceptions.HTTPError as http_err:
            flash('Login failed: invalid credentials')
        except requests.exceptions.ConnectionError as conn_err:
            flash('Connection error: please try again later')
        except requests.exceptions.Timeout as timeout_err:
            flash('Request timed out: please try again later')
        except requests.exceptions.RequestException as req_err:
            flash('An unexpected error occurred: please try again later')
        return redirect(url_for('login'))
    return render_template('login.html')


@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        user_data = request.form
        try:
            response = requests.post(f'{backend_url}/signup', json=user_data)
            # Questo genera un'eccezione per codici di stato HTTP 4xx/5xx
            response.raise_for_status()
            session['user'] = response.json()['user']
            session['token'] = response.json()['token']
            return redirect(url_for('home'))
        except requests.exceptions.HTTPError as http_err:
            flash('Signup failed: invalid credentials')
        except requests.exceptions.ConnectionError as conn_err:
            flash('Connection error: please try again later')
        except requests.exceptions.Timeout as timeout_err:
            flash('Request timed out: please try again later')
        except requests.exceptions.RequestException as req_err:
            flash('An unexpected error occurred: please try again later')
        return redirect(url_for('signup'))
    return render_template('signup.html')


@app.route('/reviews/<string:gameTitle>', methods=['GET', 'POST'])
def reviews(gameTitle):
    if request.method == 'POST':
        # Ottieni i dati del form e aggiungi il gameTitle e il rating
        review_text = request.form.get('review')
        rating = int(request.form.get('rating'))  # Converti il rating in intero
        username = session.get('user')
        review_data = {
            'username': username,
            'gameTitle': gameTitle,
            'reviewText': review_text,
            'rating': rating
        }

        headers = getHeaders()
        try:
            response = requests.post(f'{backend_url}/addReview', json=review_data, headers=headers)
            response.raise_for_status()
            flash('Review added successfully!', 'success')
        except requests.exceptions.RequestException as e:
            flash('Failed to add review. Please try again later.', 'danger')

        return redirect(url_for('game', gameTitle=gameTitle))

    else:
        # Metodo GET per ottenere le recensioni già presenti
        response = requests.get(f'{backend_url}/getReviewByGame/{gameTitle}')
        reviews = response.json()
        return render_template('game.html', gameTitle=gameTitle)
    return render_template('game.html', gameTitle=gameTitle)



@app.route('/logout')
def logout():
    session.pop('user', None)
    return redirect(url_for('home'))


if __name__ == '__main__':
    app.run(debug=True)
