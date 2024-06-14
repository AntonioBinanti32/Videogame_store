from flask import Flask, render_template, request, redirect, url_for, session, flash
import requests
import configparser

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


@app.route('/')
def home():
    try:
        if not is_user_logged_in():
            return redirect(url_for('login'))

        response = requests.get(f'{backend_url}/getGames', headers = getHeaders()) #TODO: inserire user e token per validazione
        games = response.json()
        return render_template('home.html', games=games)
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
    response = requests.get(f'{backend_url}/getGameByTitle/{gameTitle}')
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
        response = requests.post(f'{backend_url}/signup', json=user_data)
        if response.status_code == 201:
            return redirect(url_for('login'))
        else:
            flash('Signup Failed')
            return redirect(url_for('signup'))
    return render_template('signup.html')


@app.route('/reviews/<string:gameTitle>', methods=['GET', 'POST'])
def reviews(gameTitle):
    if request.method == 'POST':
        review_data = request.form
        review_data['gameTitle'] = gameTitle
        response = requests.post(f'{backend_url}/addReview', json=review_data)
        return redirect(url_for('reviews', gameTitle=gameTitle))
    else:
        response = requests.get(f'{backend_url}/getReviewByGame/{gameTitle}')
        reviews = response.json()
        return render_template('reviews.html', reviews=reviews, gameTitle=gameTitle)


@app.route('/logout')
def logout():
    session.pop('user', None)
    return redirect(url_for('home'))


if __name__ == '__main__':
    app.run(debug=True)
